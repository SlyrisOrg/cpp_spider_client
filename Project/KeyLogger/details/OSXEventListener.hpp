//
// Created by baptiste montagliani on 02/10/2017.
//

#ifndef OSX_EVENT_LISTENER_HPP
#define OSX_EVENT_LISTENER_HPP

#include <map>
#include <vector>
#include <functional>
#include <thread>
#include <ApplicationServices/ApplicationServices.h>

CGEventRef _eventCallBack(CGEventTapProxy, CGEventType type, CGEventRef event, void *data);

class OSXEventListener
{
public:
    using EventAction = std::function<void(CGEventType, CGEventRef)>;
    using EventsList = std::vector<CGEventType>;

    OSXEventListener() : _started(false)
    {}
    ~OSXEventListener()
    {
        stopListening();
    }

public:
    void registerEvent(CGEventType type, EventAction action) noexcept
    {
        if (!_started) {
            _registeredEvents[type] = action;
        }
    }

    void registerEvents(EventsList const &types, EventAction action) noexcept
    {
        if (!_started) {
            for (auto const &type : types) {
                _registeredEvents[type] = action;
            }
        }
    }

    void startListening()
    {
        if (!_started) {
            _started = true;
            _eventThread = new std::thread(&OSXEventListener::_startCoreGraphicsLoop, this);
        }
    }

    void stopListening()
    {
        if (_started && _eventLoop) {
            CFRunLoopStop(_eventLoop);
            _eventThread->join();
            _started = false;
        }
    }
        
    decltype(auto) eventForType(CGEventType type) {
        return _registeredEvents.find(type);
    }

    decltype(auto) eventEnd() {
        return _registeredEvents.end();
    }

private:
    std::map<CGEventType, EventAction> _registeredEvents;
    std::thread *_eventThread {NULL};
    CFRunLoopRef _eventLoop {NULL};
    bool _started{false};

    CGEventMask _getMaskForRegisteredEvents()
    {
        CGEventMask mask = kCGEventNull;

        for (auto const &event : _registeredEvents) {
            mask |= CGEventMaskBit(event.first);
        }
        return mask;
    }

    void _startCoreGraphicsLoop()
    {
        CGEventTapCallBack callback = &_eventCallBack;
        CGEventMask eventMask = _getMaskForRegisteredEvents();
        CFMachPortRef eventReceiver = CGEventTapCreate(kCGSessionEventTap,
                                                       kCGHeadInsertEventTap,
                                                       kCGEventTapOptionDefault,
                                                       eventMask,
                                                       callback,
                                                       this);
        if (!eventReceiver) {
            return;
        }
            
        CFRunLoopSourceRef eventRunLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault,
                                                                                  eventReceiver, 0);
        CGEventTapEnable(eventReceiver, true);

        _eventLoop = CFRunLoopGetCurrent();
        CFRunLoopAddSource(_eventLoop, eventRunLoopSource, kCFRunLoopCommonModes);

        CFRunLoopRun();
    }
};

CGEventRef _eventCallBack(CGEventTapProxy, CGEventType type, CGEventRef event, void *data) {
    
    OSXEventListener *self = (OSXEventListener *)data;
    
    auto item = self->eventForType(type);
    
    if (item != self->eventEnd()) {
        item->second(type, event);
    }
    return event;
}

#endif //OSX_EVENT_LISTENER_HPP
