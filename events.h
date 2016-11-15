#ifndef Events_events_h
#define Events_events_h
#include <vector>
#include <functional>

template<class Sender, typename... arguments>
class event{
private:
    class event_handler{
    public:
        event_handler(int tag) : tag(tag) {}
        virtual ~event_handler() {}
        bool isEqual(event_handler *other) {
            if (!other) return false;
            if (other->tag != this->tag) return false;
            return this->checkEqual(other);
        }
        virtual void notify(Sender *s, arguments... args) = 0;
    protected:
        virtual bool checkEqual(event_handler *other) = 0;
    private:
        int tag;
    };
    
    class event_handler_list {
    public:
        void add(event_handler *handler) {
            handlers.push_back(handler);
        }
        void remove(event_handler *handler) {
            auto it = handlers.begin();
            auto end = handlers.end();
            for(; it != end; ++it) {
                if((*it)->isEqual(handler)) {
                    break;
                }
            }
            if(it != end) {
                event_handler *handler = *it;
                handlers.erase(it);
                delete handler;
            }
        }
        void notify(Sender *s, arguments... args) {
            for(auto handler : handlers) {
                handler->notify(s, args...);
            }
        }
    private:
        std::vector<event_handler*> handlers;
    };
    
    template<class O, class F>
    class instance_event_handler : public event_handler {
    public:
        instance_event_handler(O o, F f) : event_handler(1), o(o), f(f){}
        virtual void notify(Sender *s, arguments... args) {
            (o->*f)(s, args...);
        }
    protected:
        virtual bool checkEqual(event_handler *other) {
            auto handler = dynamic_cast<instance_event_handler*>(other);
            return this->o == handler->o && this->f == handler->f;
        }
    private:
        O o;
        F f;
    };
    
    class static_event_handler : public event_handler{
    public:
        typedef void (*F)(Sender*, arguments...);
        static_event_handler(F f) : event_handler(2), f(f){}
        virtual void notify(Sender *s, arguments... args){
            f(s, args...);
        }
    protected:
        virtual bool checkEqual(event_handler *other) {
            auto handler = dynamic_cast<static_event_handler*>(other);
            return this->f == handler->f;
        }
    private:
        F f;
    };
    
    class function_event_handler : public event_handler{
    public:
        typedef std::function<void (Sender*, arguments...)> F;
        function_event_handler(F f) : event_handler(3), f(f){}
        virtual void notify(Sender *s, arguments... args){
            f(s, args...);
        }
    protected:
        virtual bool checkEqual(event_handler *other) {
            return false;
        }
    private:
        F f;
    };
    
    Sender *s;
    event_handler_list handler_list;
    friend Sender;

protected:
    event(Sender *s) : s(s) {}
    void operator()(arguments... args) {
        handler_list.notify(s, args...);
    }

public:
    template<class O, class F>
    void operator +=(std::pair<O, F> p) {
        handler_list.add(new instance_event_handler<O, F>(p.first, p.second));
    }
    
    template<class O, class F>
    void operator -=(std::pair<O, F> p) {
        auto handler = instance_event_handler<O, F>(p.first, p.second);
        handler_list.remove(&handler);
    }
    
    void operator +=(void (*f)(Sender*, arguments...)) {
        handler_list.add(new static_event_handler(f));
    }
    
    void operator -=(void (*f)(Sender*, arguments...)) {
        auto handler = static_event_handler(f);
        handler_list.remove(&handler);
    }
    
    
    void operator +=(std::function<void (Sender*, arguments...)> f) {
        handler_list.add(new function_event_handler(f));
    }
    
    template<class F>
    void operator -=(std::function<void (Sender*, arguments...)> f) {
    }
};

template<class O, class F>
std::pair<O, F> event_handler(O o, F f){
    return std::pair<O, F>(o, f);
}

#endif
