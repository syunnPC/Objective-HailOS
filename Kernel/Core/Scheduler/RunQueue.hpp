#pragma once

#include "Thread.hpp"
#include "Panic.hpp"

namespace Kernel::Sched
{
    class RunQueue
    {
    public:
        Thread* Head{ nullptr };
        Thread* Tail{ nullptr };

        bool Empty() const noexcept
        {
            return Head == nullptr;
        }

        void Push(Thread* t) noexcept
        {
            if (!t)
            {
                PANIC(Status::STATUS_ERROR, 0);
            }
            t->Next = nullptr;
            t->Prev = Tail;
            t->InRunQueue = true;
            if (Tail != nullptr)
            {
                Tail->Next = t;
            }
            else
            {
                Head = t;
            }

            Tail = t;
        }

        Thread* PopFront() noexcept
        {
            Thread* t = Head;
            if (t == nullptr)
            {
                return nullptr;
            }
            Head = t->Next;
            if (Head != nullptr)
            {
                Head->Prev = nullptr;
            }
            else
            {
                Tail = nullptr;
            }

            t->Next = t->Prev = nullptr;

            t->InRunQueue = false;

            return t;
        }
    };
}