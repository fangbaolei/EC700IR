/* ======================================================================== */
/*  IMG_idct_8x8_12q4   -- Natural C version of IMG_idct_8x8_12q4().        */
/* ======================================================================== */
void IMG_idct_8x8_12q4(short *idct_data, unsigned num_idcts)
{
    /* -------------------------------------------------------------------- */
    /*  Cosine Constants (Q16, scaled down by sqrt(2)).                     */
    /* -------------------------------------------------------------------- */
    const unsigned short C0 = 0xB505;
    const unsigned short C1 = 0xB18B, C2 = 0xA73D;
    const unsigned short C3 = 0x9683, C5 = 0x6492;
    const unsigned short C6 = 0x4546, C7 = 0x2351;

    /* -------------------------------------------------------------------- */
    /*  Intermediate values (used in both loops).                           */
    /* -------------------------------------------------------------------- */
    short F0, F1, F2, F3, F4, F5, F6, F7;  /* stage 0        */
    short P0, P1, R0, R1, Q0, Q1, S0, S1;  /* stage 1        */
    short p0, p1, r0, r1, q0, q1, s0, s1;  /* stage 2        */
    short g0, g1, g2, g3, h0, h1, h2, h3;  /* stage 3        */
    short f0, f1, f2, f3, f4, f5, f6, f7;  /* stage 4        */
    short f0r,f1r,f2r,f3r,f4r,f5r,f6r,f7r; /* rounded        */
    int   f0s,f1s,f2s,f3s,f4s,f5s,f6s,f7s; /* saturated      */
    int   f0t,f1t,f2t,f3t,f4t,f5t,f6t,f7t; /* truncated      */
    int   i, j;                            /* loop counts    */
    short (*idct)[8][8] = (short (*)[8][8])idct_data;

    if (!num_idcts) return;

    /* -------------------------------------------------------------------- */
    /*  Vertical Pass                                                       */
    /*                                                                      */
    /*  This pass performs a single 8-pt IDCT per iteration.  Inputs        */
    /*  are in 12Q4 format, and results of this pass are in 11Q5            */
    /*  format. (Actually, the results are halfway between 11Q5 and         */
    /*  12Q4 due to the scaling by sqrt(2).)                                */
    /*                                                                      */
    /*  The outer loop steps between IDCT blocks, whereas the inner         */
    /*  loop focuses on columns within each IDCT block.                     */
    /* -------------------------------------------------------------------- */
    for (i = 0; i < num_idcts; i++)
    {
        for (j = 0; j < 8; j++)
        {
            /* ------------------------------------------------------------ */
            /*  Stage 0:  Load in frequency-domain coefficients.            */
            /* ------------------------------------------------------------ */
            F0 = idct[i][0][j];
            F1 = idct[i][1][j];
            F2 = idct[i][2][j];
            F3 = idct[i][3][j];
            F4 = idct[i][4][j];
            F5 = idct[i][5][j];
            F6 = idct[i][6][j];
            F7 = idct[i][7][j];

            /* ------------------------------------------------------------ */
            /*  Stage 1 of signal flow graph.                               */
            /* ------------------------------------------------------------ */
            P0 = F0;                P1 = F4;
            R1 = F2;                R0 = F6;

            Q1 = (F1*C7 - F7*C1 + 0x8000) >> 16;
            Q0 = (F5*C3 - F3*C5 + 0x8000) >> 16;
            S0 = (F5*C5 + F3*C3 + 0x8000) >> 16;
            S1 = (F1*C1 + F7*C7 + 0x8000) >> 16;

            /* ------------------------------------------------------------ */
            /*  Stage 2 of signal flow graph.                               */
            /* ------------------------------------------------------------ */
            p0 = ((int)P0 + (int)P1 + 1 ) >> 1;
            p1 = ((int)P0 - (int)P1     ) >> 1;
            r1 = (R1*C6 - R0*C2 + 0x8000) >> 16;
            r0 = (R1*C2 + R0*C6 + 0x8000) >> 16;

            s1 = (S1 + S0);         q1 = (Q1 + Q0);
            s0 = (S1 - S0);         q0 = (Q1 - Q0);

            /* ------------------------------------------------------------ */
            /*  Stage 3 of signal flow graph.                               */
            /* ------------------------------------------------------------ */
            g0 = (p0 + r0);         g1 = (p1 + r1);
            h0 = (p0 - r0);         h1 = (p1 - r1);

            h2 = s1;                g2 = q1;
            g3 = (s0*C0 - q0*C0 + 0x8000) >> 16;
            h3 = (s0*C0 + q0*C0 + 0x8000) >> 16;

            /* ------------------------------------------------------------ */
            /*  Stage 4 of signal flow graph.                               */
            /* ------------------------------------------------------------ */
            f0 = (g0 + h2);         f7 = (g0 - h2);
            f1 = (g1 + h3);         f6 = (g1 - h3);
            f2 = (h1 + g3);         f5 = (h1 - g3);
            f3 = (h0 + g2);         f4 = (h0 - g2);

            /* ------------------------------------------------------------ */
            /*  Stage 5:  Write sample-domain results.                      */
            /* ------------------------------------------------------------ */
            idct[i][0][j] = f0;
            idct[i][1][j] = f1;
            idct[i][2][j] = f2;
            idct[i][3][j] = f3;
            idct[i][4][j] = f4;
            idct[i][5][j] = f5;
            idct[i][6][j] = f6;
            idct[i][7][j] = f7;
        }
    }

    /* -------------------------------------------------------------------- */
    /*  Horizontal Pass                                                     */
    /*                                                                      */
    /*  This performs one IDCT per iteration on the 11Q5 results from       */
    /*  the previous pass.  Both horizontal and vertical passes are         */
    /*  scaled down by sqrt(2) -- the net effect of which is that the       */
    /*  IDCT results generated by this pass (prior to saturation) are       */
    /*  also 11Q5 results, only with no sqrt(2) factors remaining.          */
    /*                                                                      */
    /*  The IDCT butterflies in this pass are identical to the ones in      */
    /*  the vertical pass, except for an additional rounding value          */
    /*  which is added into the DC term early in the flow graph.            */
    /*                                                                      */
    /*  The 11Q5 sample-domain terms are saturated to 9Q7 values, and       */
    /*  then truncated to 9Q0 results before storing.                       */
    /*                                                                      */
    /*  The outer loop steps between IDCT blocks, whereas the inner         */
    /*  loop focuses on rows within each IDCT block.                        */
    /* -------------------------------------------------------------------- */
    for (i = 0; i < num_idcts; i++)
    {
        for (j = 0; j < 8; j++)
        {
            /* ------------------------------------------------------------ */
            /*  Stage 0:  Load in frequency-domain coefficients.            */
            /* ------------------------------------------------------------ */
            F0 = idct[i][j][0];
            F1 = idct[i][j][1];
            F2 = idct[i][j][2];
            F3 = idct[i][j][3];
            F4 = idct[i][j][4];
            F5 = idct[i][j][5];
            F6 = idct[i][j][6];
            F7 = idct[i][j][7];

            /* ------------------------------------------------------------ */
            /*  Stage 1 of signal flow graph.                               */
            /* ------------------------------------------------------------ */
            P0 = F0;                P1 = F4;
            R1 = F2;                R0 = F6;

            Q1 = (F1*C7 - F7*C1 + 0x8000) >> 16;
            Q0 = (F5*C3 - F3*C5 + 0x8000) >> 16;
            S0 = (F5*C5 + F3*C3 + 0x8000) >> 16;
            S1 = (F1*C1 + F7*C7 + 0x8000) >> 16;

            /* ------------------------------------------------------------ */
            /*  Stage 2 of signal flow graph.                               */
            /* ------------------------------------------------------------ */
            p0 = (((int)P0 + (int)P1 + 1) >> 1) + 15;
            p1 = (((int)P0 - (int)P1    ) >> 1) + 16;
            r1 = (R1*C6 - R0*C2 + 0x8000) >> 16;
            r0 = (R1*C2 + R0*C6 + 0x8000) >> 16;

            s1 = (S1 + S0);         q1 = (Q1 + Q0);
            s0 = (S1 - S0);         q0 = (Q1 - Q0);

            /* ------------------------------------------------------------ */
            /*  Stage 3 of signal flow graph.                               */
            /* ------------------------------------------------------------ */
            g0 = (p0 + r0);         g1 = (p1 + r1);
            h0 = (p0 - r0);         h1 = (p1 - r1);

            h2 = s1;                g2 = q1;
            g3 = (s0*C0 - q0*C0 + 0x8000) >> 16;
            h3 = (s0*C0 + q0*C0 + 0x8000) >> 16;

            /* ------------------------------------------------------------ */
            /*  Stage 4 of signal flow graph.                               */
            /* ------------------------------------------------------------ */
            f0 = (g0 + h2);         f7 = (g0 - h2);
            f1 = (g1 + h3);         f6 = (g1 - h3);
            f2 = (h1 + g3);         f5 = (h1 - g3);
            f3 = (h0 + g2);         f4 = (h0 - g2);

            /* ------------------------------------------------------------ */
            /*  Stage 4.1:  Q-pt adjust: Bit 15 is a don't-care.            */
            /* ------------------------------------------------------------ */
            f0r = f0 + f0;          f7r = f7 + f7;
            f1r = f1 + f1;          f6r = f6 + f6;
            f2r = f2 + f2;          f5r = f5 + f5;
            f3r = f3 + f3;          f4r = f4 + f4;

            /* ------------------------------------------------------------ */
            /*  Stage 4.2:  Saturate results to 9Q6.                        */
            /* ------------------------------------------------------------ */
            f0s = f0r > 0x3FFF ? 0x3FFF : f0r < -0x4000 ? -0x4000 : f0r;
            f1s = f1r > 0x3FFF ? 0x3FFF : f1r < -0x4000 ? -0x4000 : f1r;
            f2s = f2r > 0x3FFF ? 0x3FFF : f2r < -0x4000 ? -0x4000 : f2r;
            f3s = f3r > 0x3FFF ? 0x3FFF : f3r < -0x4000 ? -0x4000 : f3r;
            f4s = f4r > 0x3FFF ? 0x3FFF : f4r < -0x4000 ? -0x4000 : f4r;
            f5s = f5r > 0x3FFF ? 0x3FFF : f5r < -0x4000 ? -0x4000 : f5r;
            f6s = f6r > 0x3FFF ? 0x3FFF : f6r < -0x4000 ? -0x4000 : f6r;
            f7s = f7r > 0x3FFF ? 0x3FFF : f7r < -0x4000 ? -0x4000 : f7r;

            /* ------------------------------------------------------------ */
            /*  Stage 4.3:  Truncate results to 9Q0.                        */
            /* ------------------------------------------------------------ */
            f0t = f0s >> 6;         f7t = f7s >> 6;
            f1t = f1s >> 6;         f6t = f6s >> 6;
            f2t = f2s >> 6;         f5t = f5s >> 6;
            f3t = f3s >> 6;         f4t = f4s >> 6;

            /* ------------------------------------------------------------ */
            /*  Stage 5:  Store sample-domain results.                      */
            /* ------------------------------------------------------------ */
            idct[i][j][0] = f0t;
            idct[i][j][1] = f1t;
            idct[i][j][2] = f2t;
            idct[i][j][3] = f3t;
            idct[i][j][4] = f4t;
            idct[i][j][5] = f5t;
            idct[i][j][6] = f6t;
            idct[i][j][7] = f7t;
        }
    }

    return;
}

/* ======================================================================== */
/*  End of file:  img_idct_8x8_12q4.c                                       */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
