/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_INPUTPIN_HPP
#define _CORE_INPUTPIN_HPP

namespace core {

class InputPin
{
public:
   // enum State {
   //    LOW,
   //    HIGH
   // };

   virtual ~InputPin(){}
   virtual bool get() = 0;
};

class NullInputPin: public InputPin
{
public:
   virtual bool get();

private:
   bool input_state;
};

class OnDelayButton
{
public:
    OnDelayButton(InputPin& pin, unsigned on_delay_counts)
        : pin_(pin),
          on_delay_counts_(on_delay_counts)
    {
    }

    // call once per second
    void update()
    {
        bool pressed = pin_.get();

        if (!pressed)
        {
            counter_ = 0;
            on_ = false;
            fired_ = false;   // re-arm when released
            return;
        }

        if (!on_)
        {
            if (counter_ < on_delay_counts_)
            {
                counter_++;
            }

            if (counter_ >= on_delay_counts_)
            {
                on_ = true;
            }
        }
    }

    bool isOn() const
    {
        return on_;
    }

    // returns true once per ON cycle
    bool consumeOn()
    {
        if (on_ && !fired_)
        {
            fired_ = true;
            return true;
        }
        return false;
    }

private:
    InputPin& pin_;

    unsigned on_delay_counts_;
    unsigned counter_ = 0;

    bool on_ = false;
    bool fired_ = false;
};

}

#endif // _CORE_INPUTPIN_HPP
