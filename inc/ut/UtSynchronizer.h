/* Copyright © Artur Maziarek MMXX
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#ifndef UTSYNCHRONIZER_H_
#define UTSYNCHRONIZER_H_

#include <string>
#include <mutex>
#include <map>
#include <memory>
#include <chrono>
#include <condition_variable>

template <class Id>
class UtSynchronizer
{
public:
    void setup(Id id);
    void waitFor(Id id);
    void notify(Id id);
    void reset(Id id);

private:
    static constexpr int maxWaitTime = 5; // in seconds

    std::map<Id, std::shared_ptr<std::mutex>> notifyControl;
    std::map<Id, std::shared_ptr<std::unique_lock<std::mutex>>> notifyLock;
    std::map<Id, std::condition_variable> eventReady;
};


template <class Id>
void UtSynchronizer<Id>::setup(Id id) {
    notifyLock[id] = nullptr;
    notifyControl[id] = std::make_shared<std::mutex>();
    notifyLock[id] = std::make_shared<std::unique_lock<std::mutex>>(*(notifyControl.at(id)));
}

template <class Id>
void UtSynchronizer<Id>::waitFor(Id id) {
    if (notifyLock.count(id) == 0) {
        setup(id);
    }
    eventReady[id].wait_for(*(notifyLock[id]), std::chrono::seconds((int)maxWaitTime));
}

template <class Id>
void UtSynchronizer<Id>::notify(Id id) {
    if (notifyControl.count(id) > 0) {
        std::lock_guard<std::mutex> notifyLock{*(notifyControl.at(id))};
        eventReady[id].notify_all();
    }
}

template <class Id>
void UtSynchronizer<Id>::reset(Id id) {
    if (notifyLock.count(id) > 0) {
        notifyLock.at(id)->unlock();
    }
}

#endif // UTSYNCHRONIZER_H_
