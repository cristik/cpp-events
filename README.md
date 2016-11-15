# cpp-events
Events basis for C++

Usage example below:
```cpp
#include "events.h"

class Subject {
public:
    // void(Subject *sender)
    event<Subject> onDataChanged;
    // void(Subject *sender, int age)
    event<Subject, int> onNewAgeReceived;
    // void(Subject *sender, int oldAge, int newAge)
    event<Subject, int, int> onAgeChanged;
    
    // need to pass the sender param when constructing the events
    Subject() : onDataChanged(this), onNewAgeReceived(this), onAgeChanged(this) {}
    
    void updateAge(int newAge) {
        int oldAge = age;
        age = newAge;
        // sender, aka this will be automatically passed to the subscribers
        // the () operator is protected, however Subject is friend to event
        // trying to raise the event from outside the class is not possible
        onDataChanged();
        onNewAgeReceived(age);
        onAgeChanged(oldAge, newAge);
    }
private:
    int age;
};

class Observer {
private:
    Subject *subject;
public:
    Observer(Subject *aSubject) : subject(aSubject) {
        // instance methods need to use the event_handler() helper function
        subject->onDataChanged += event_handler(this, &Observer::subjectChanged);
        
        // static class methods, or global ones can also be added
        subject->onAgeChanged += &Observer::subjectAgeChanged;
        
        // can also add lambdas (however those can't be removed)
        subject->onNewAgeReceived += [=](Subject *sender, int age) {
            printf("Subject received new age: %d\n", age);
        };
        
        // non-capturing lamdba's need a workardound otherwise the compiler will
        // complain about += being ambigous, calling the unary "+" operator will do the trick
        subject->onNewAgeReceived += +[](Subject *sender, int age) {
            // do whatever is needed
        };
    }
    
    virtual ~Observer() {
        // remove the attached event handlers
        subject->onDataChanged -= event_handler(this, &Observer::subjectChanged);
        subject->onAgeChanged -= &Observer::subjectAgeChanged;
    }
    
    void subjectChanged(Subject *sender) {
        printf("Subject changed\n");
    }
    
    static void subjectAgeChanged(Subject *sender, int oldAge, int newAge) {
        printf("Subject age changed from %d to %d\n", oldAge, newAge);
    }
};
```
