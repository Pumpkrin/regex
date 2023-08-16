#include "stator.hpp"
int main(){
    constexpr stator_t< 
                        rankify<0, state_t<'a'>>,
                        rankify<1, state_t<'b'>>,
                        rankify<2, state_t<'a', repetition_t>>,
                        rankify<3, state_t<'c'>>
                      > stator;
    stator.process('a');
    stator.process('c');
    stator.process('b');
    stator.process('d');
    stator.process('a');
    stator.process('b');
    stator.process('a');
    stator.process('a');
    stator.process('a');
    stator.process('a');
    stator.process('c');
}
