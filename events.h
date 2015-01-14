#ifndef Events_events_h
#define Events_events_h
#include <vector>
#include <functional>

template<typename...>
class event { };

template<typename S>
class event<S> {
private:
    class event_handler {
    public:
        event_handler(int tag) : tag(tag) {}
        void operator()(S *s){ execute(s); }
        virtual event_handler *heapify() = 0;
        bool isEqual(event_handler *other) {
            if (!other) return false;
            if (other->tag != this->tag) return false;
            return this->checkEqual(other);
        };
    protected:
        int tag;
        virtual void execute(S *s) = 0;
        virtual bool checkEqual(event_handler *other) = 0;
    };

    class event_handler_list {
    public:
        std::vector<event_handler*> handlers;
        void add(event_handler *handler) {
            handlers.push_back(handler);
        }
        void remove(event_handler *handler, bool freePtr = false) {

        }
    };

    template<class O, class C>
    class instance_event_handler : public event_handler{
    public:
        typedef void (C::*F)(S*);
        instance_event_handler(O *o, F f) :o(o), f(f){}
        virtual event_handler *heapify(){
            return new instance_event_handler<O, C>(o, f);
        }
    protected:
        virtual void execute(S *s){
            (o->*f)(s);
        }
    private:
        C *o;
        F f;
    };

#ifdef WIN_RT
    template<class O, class F>
    class managed_event_handler : public event_handler{
    public:
        //typedef void (C::*F)(S*, A);
        managed_event_handler(O ^o, F f) : event_handler(4), o(o), fn(f){}
        virtual event_handler *heapify(){
            return new managed_event_handler<O, F>(o, fn);
        }
    protected:
        virtual void execute(S *s){
            (o->*fn)(s);
        }
        virtual bool checkEqual(event_handler *other){
            auto handler = dynamic_cast<managed_event_handler*>(other);
            return this->o == handler->o && this->fn == handler->fn;
        }
    private:
        O ^o;
        F fn;
    };
#endif

    class static_event_handler : public event_handler{
    public:
        typedef void(*F)(S*);
        static_event_handler(F f) : f(f){}

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

    class function_event_handler : public event_handler{
    public:
        typedef std::function<void(S*)> F;
        function_event_handler(F f) : f(f){}
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
    event_handler_list handler_list;
    friend S;

protected:
    void operator()(){
        for (auto handler : handler_list.handlers){
            (*handler)(s);
        }
    }

    event(S *s){
        this->s = s;
    }

public:
    typedef void(*F)(S*);

    void operator +=(F f){
        handler_list.add(new static_event_handler(f));
    }

    void operator -=(F f){
        auto handler = static_event_handler(f)
        handler_list.remove(&handler);
    }

    template<class O, class C>
    void operator +=(std::pair<O*, void (C::*)(S*)>p){
        handler_list.add(new instance_event_handler<O, C>(p.first, p.second));
    }

    template<class O, class C>
    void operator -=(std::pair<O*, void (C::*)(S*)>p){
        auto handler = instance_event_handler<O, C>(p.first, p.second);
        handler_list.remove(&handler);
    }

#ifdef WIN_RT
    template<class O, class F>
    void operator +=(std::pair<O^, F>p){
        handler_list.add(new managed_event_handler<O, F>(p.first, p.second));
    }

    template<class O, class F>
    void operator -=(std::pair<O^, F>p){
        handler_list.remove(new managed_event_handler<O, F>(p.first, p.second));
    }
#endif

};

template<class S, typename A>
class event<S, A> {
private:
    class event_handler{
    public:
        event_handler(int tag) : tag(tag){}
        void operator()(S *s, A a){ execute(s, a); }
        virtual event_handler *heapify() = 0;
        bool isEqual(event_handler *other) {
            if (!other) return false;
            if (other->tag != this->tag) return false;
            return this->checkEqual(other);
        };
    protected:
        int tag;
        virtual void execute(S *s, A a) = 0;
        virtual bool checkEqual(event_handler *other) = 0;
    };

    class event_handler_list {
    public:
        std::vector<event_handler*> handlers;
        void add(event_handler *handler) {
            handlers.push_back(handler);
        }
        void remove(event_handler *handler, bool freePtr=false) {

        }
    };

    template<class O, class C>
    class instance_event_handler : public event_handler{
    public:
        typedef void (C::*F)(S*, A);
        instance_event_handler(O *o, F f) : event_handler(1), o(o), fn(f){}
        virtual event_handler *heapify(){
            return new instance_event_handler<O, C>(o, fn);
        }
        virtual bool isEqual(event_handler *other) {
            if (this->tag != other->tag) return false;
            auto handler = dynamic_cast<instance_event_handler*>(other);
            return this->o == handler->o && this->fn == handler->fn;
        }
    protected:
        virtual void execute(S *s, A a){
            (o->*fn)(s, a);
        }
    private:
        C *o;
        F fn;
    };

#ifdef WIN_RT
    template<class O, class F>
    class managed_event_handler : public event_handler{
    public:
        //typedef void (C::*F)(S*, A);
        managed_event_handler(O ^o, F f) : event_handler(4), o(o), fn(f){}
        virtual event_handler *heapify(){
            return new managed_event_handler<O, F>(o, fn);
        }        
    protected:
        virtual void execute(S *s, A a){
            (o->*fn)(s, a);
        }
        virtual bool checkEqual(event_handler *other){
            auto handler = dynamic_cast<managed_event_handler*>(other);
            return this->o == handler->o && this->fn == handler->fn;
        }
    private:
        O ^o;
        F fn;
    };
#endif

    class static_event_handler : public event_handler{
    public:
        typedef void(*F)(S*, A);
        static_event_handler(F f) : event_handler(2), f(f){}
        virtual event_handler *heapify(){
            return new static_event_handler(f);
        }
        virtual bool isEqual(event_handler *other) {
            if (this->tag != other->tag) return false;
            auto handler = dynamic_cast<static_event_handler*>(other);
            return this->f == handler->f;
        }
    protected:
        virtual void execute(S *s, A a){
            (*f)(s, a);
        }
    private:
        F f;
    };

    class function_event_handler : public event_handler{
    public:
        typedef std::function<void(S*, A)> F;
        function_event_handler(F f) : event_handler(3), f(f){ }
        virtual event_handler *heapify(){
            return new function_event_handler(f);
        }
    protected:
        virtual void execute(S *s, A a){
            f(s, a);
        }
        virtual bool checkEqual(event_handler *other) {
            // functions can't be compared
            return false;
        }
    private:
        F f;
    };

    S *s;
    event_handler_list handler_list;
    friend S;

protected:
    event(S *s){
        this->s = s;
    }

    void operator()(A a){
        for (auto handler : handler_list.handlers){
            (*handler)(s, a);
        }
    }

public:
    typedef void(*F)(S*, A);

    void operator +=(F f){
        handler_list.add(new static_event_handler(f));
    }

    void operator -=(F f){
        auto handler = new static_event_handler(f);
        handler_list.remove(handler, true);
        delete handler;
    }

    void operator +=(std::function<void(S*, A)> f) {
        handler_list.add(new function_event_handler(f));
    }

    template<class O, class C>
    void operator +=(std::pair<O*, void (C::*)(S*, A)>p){
        handler_list.add(new instance_event_handler<O, C>(p.first, p.second));
    }

    template<class O, class C>
    void operator -=(std::pair<O*, void (C::*)(S*)>p){
        auto handler = instance_event_handler<O, C>(p.first, p.second);
        handler_list.remove(&handler, true);
    }

#ifdef WIN_RT
    template<class O, class F>
    void operator +=(std::pair<O^, F>p){
        handler_list.add(new managed_event_handler<O, F>(p.first, p.second));
    }

    template<class O, class F>
    void operator -=(std::pair<O^, F>p){
        handler_list.remove(new managed_event_handler<O, F>(p.first, p.second));
    }
#endif

};

template<class O, class C, class S>
inline std::pair<O*, void (C::*)(S*)> event_handler(O *o, void (C::*f)(S*)){
    return std::pair<O*, void (C::*)(S*)>(o, f);
}

template<class O, class C, class S, typename A>
inline std::pair<O*, void (C::*)(S*, A)> event_handler(O *o, void (C::*f)(S*, A)){
    return std::pair<O*, void (C::*)(S*, A)>(o, f);
}

#ifdef WIN_RT
template<class O, class F>
std::pair<O^, F> event_handler(O ^o, F f){
    return std::pair<O^, F>(o, f);
}
#endif

#endif
