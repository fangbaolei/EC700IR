#ifndef AUTOPTR
#define AUTOPTR

//智能指针类
/* sample

int main() 
{
    AutoPtr<int> p = new int;
    *p = 100;
    printf("%d\n", *p);

    AutoPtr<string> sp = new string;
    *sp = "hello world";
    printf("%s\n", sp->c_str());
    return 0;
}

*/
template<class T>
class AutoPtr 
{
public :
    AutoPtr(T* p = 0) : pointee(p) {} //默认构造函数

    template<class U>
    AutoPtr(AutoPtr<U>& rhs) : pointee(rhs.release()) {}//复制构造函数

    ~AutoPtr() { if(pointee) delete pointee; }

    template<class U>
    AutoPtr<T>& operator=(AutoPtr<U>& rhs) //赋值函数
	{ 
        if (this != &rhs) 
		{
            reset(rhs.release());
        }
        return *this;
    }

    T& operator*() const {return *pointee;} 
    
    T* operator->() const {return pointee;}

    T* get() const {return pointee;} //获取dumb pointer

    T* release()  //释放dumb pointer 的拥有权，并返回其值
	{
        T* oldPointee = pointee;
        pointee = 0;
        return oldPointee;
    } 
    
    void reset(T* p=0) //重复置p指针
	{ 
        if (pointee != p) 
		{
            if(pointee) delete pointee;
            pointee = p;
        }
    }

private :
    T* pointee;
};

#endif //AUTOPTR



