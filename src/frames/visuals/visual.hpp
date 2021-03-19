#ifndef VISUAL_HPP_INCLUDED
#define VISUAL_HPP_INCLUDED
class Visual {
  public:
    virtual void loop(void) = 0;
//    void Visual(void);
    virtual ~Visual() = 0;
};
#endif // VISUAL_HPP_INCLUDED