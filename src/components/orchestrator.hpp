#ifndef ORCHESTRATOR_HPP_INCLUDED
#define ORCHESTRATOR_HPP_INCLUDED

#include "frames/frame.hpp"

class Orchestrator
{
  public:
    void setup();
    void loop(void);
    void react(FrameEvent* e);

    static Orchestrator* Instance() {
      if (!instance) {
        instance = new Orchestrator();
      }

      return instance;
    }

  private:
    static Orchestrator* instance;
    Orchestrator () { } /* verhindert, dass ein Objekt von außerhalb von N erzeugt wird. -- protected, wenn man von der Klasse noch erben möchte */
    Orchestrator ( const Orchestrator& ); /* verhindert, dass eine weitere Instanz via Kopie-Konstruktor erstellt werden kann */
    Orchestrator & operator = (const Orchestrator &); //Verhindert weitere Instanz durch Kopie
    ~Orchestrator () { }

    void switchFrame(FrameEvent* e);

    unsigned long frameActiveSince;
    FrameEvent* lastEvent;
    FrameEvent* currentEvent;
    FrameEvent* nextEvent;

    Frame* currentFrame;
};

#endif // ORCHESTRATOR_HPP_INCLUDED