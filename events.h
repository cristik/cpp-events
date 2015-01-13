#ifndef Events_events_h
#define Events_events_h
#include <vector>
#include <functional>

template<typename...>
class event { };

template<typename S>
class event<S> {
private:
    class event_handler{
    public:
        void operator()(S *s){ execute(s);}
        virtual event_handler *heapify() = 0;
    protected:
        virtual void execute(S *s) = 0;
    };
    
    template<class O, class C>
    class instance_event_handler: public event_handler{
    public:
        typedef void (C::*F)(S*);
        instance_event_handler(O *o, F f):o(o), f(f){}
        virtual event_handler *heapify(){
            return new instance_event_handler<O,C>(o,f);
        }
    protected:
        virtual void execute(S *s){
            (o->*f)(s);
        }
    private:
        C *o;
        F f;
    };
    
    class static_event_handler: public event_handler{
    public:
        typedef void (*F)(S*);
        static_event_handler(F f): f(f){}
        
        virtual event_handler *heapify(){
            return new static_event_handler(f);
        }
    protected:
        virtual void execute(S *s){
            (*f)(s);
        }
    private:
        F f;
    };
    
    class function_event_handler: public event_handler{
    public:
        typedef std::function<void(S*)> F;
        function_event_handler(F f): f(f){}
        virtual event_handler *heapify(){
            return new function_event_handler(f);
        }
    protected:
        virtual void execute(S *s){
            f(s);
        }
    private:
        F f;
    };
    
    S *s;
    std::vector<event_handler*> handlers;
    friend S;
    
protected:
    void operator()(){
        for(auto handler: handlers){
            (*handler)(s);
        }
    }
    
    event(S *s){
        this->s = s;
    }
    
public:
    typedef void (*F)(S*);
    
    void operator +=(F f){
        handlers.push_back(new static_event_handler(f));
    }
    
    template<class O, class C>
    void operator +=(std::pair<O*,void (C::*)(S*)>p){
        handlers.push_back(new instance_event_handler<O, C>(p.first, p.second));
    }
    
    void operator -=(F f){
        handlers.erase(std::remove(handlers.begin(), handlers.end(), f), handlers.end());
    }
    
    template<class O, class C>
    void operator -=(std::pair<O*,void (C::*)(S*)>p){
        handlers.erase(std::remove(handlers.begin(), handlers.end(), p), handlers.end());
    }
};

template<class S, typename A>
class event<S,A> {
private:
    class event_handler{
    public:
        void operator()(S *s, A a){ execute(s, a);}
        virtual event_handler *heapify() = 0;
    protected:
        virtual void execute(S *s, A a) = 0;
    };
    
    template<class O, class C>
    class instance_event_handler: public event_handler{
    public:
        typedef void (C::*F)(S*,A);
        instance_event_handler(O *o, F f):o(o), fn(f){}
        virtual event_handler *heapify(){
            return new instance_event_handler<O,C>(o,fn);
        }
    protected:
        virtual void execute(S *s, A a){
            (o->*fn)(s,a);
        }
    private:
        C *o;
        F fn;
    };
    
    class static_event_handler: public event_handler{
    public:
        typedef void (*F)(S*, A);
        static_event_handler(F f): f(f){}
        virtual event_handler *heapify(){
            return new static_event_handler(f);
        }
    protected:
        virtual void execute(S *s, A a){
            (*f)(s, a);
        }
    private:
        F f;
    };
    
    class function_event_handler: public event_handler{
    public:
        typedef std::function<void(S*,A)> F;
        function_event_handler(F f): f(f){}
        virtual event_handler *heapify(){
            return new function_event_handler(f);
        }
    protected:
        virtual void execute(S *s, A a){
            f(s, a);
        }
    private:
        F f;
    };
    
    S *s;
    std::vector<event_handler*> handlers;
    friend S;
    
protected:
    event(S *s){
        this->s = s;
    }
    
    void operator()(A a){
        for(auto handler: handlers){
            (*handler)(s,a);
        }
    }
    
public:
    typedef void (*F)(S*,A);
    
    void operator +=(F f){
        handlers.push_back(new static_event_handler(f));
    }
    
    void operator +=(std::function<void(S*,A)> f) {
        handlers.push_back(new function_event_handler(f));
    }
    
    template<class O, class C>
    void operator +=(std::pair<O*,void (C::*)(S*,A)>p){
        handlers.push_back(new instance_event_handler<O, C>(p.first, p.second));
    }
    
    void operator -=(F f){
        handlers.erase(std::remove(handlers.begin(), handlers.end(), f), handlers.end());
    }
    
    template<class O, class C>
    void operator -=(std::pair<O*,void (C::*)(S*)>p){
        handlers.erase(std::remove(handlers.begin(), handlers.end(), p), handlers.end());
    }
};

template<class O, class C, class S>
inline std::pair<O*,void (C::*)(S*)> event_handler(O *o, void (C::*f)(S*)){
    return std::pair<O*,void (C::*)(S*)>(o,f);
}

template<class O, class C, class S, typename A>
inline std::pair<O*,void (C::*)(S*,A)> event_handler(O *o, void (C::*f)(S*,A)){
    return std::pair<O*,void (C::*)(S*,A)>(o,f);
}

#endif
