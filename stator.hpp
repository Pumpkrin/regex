#ifndef stator_h
#define stator_h

#include <variant>
#include <iostream>

namespace details{
struct failure_flag{static constexpr uint8_t shift = 0;};
struct repetition_flag{static constexpr uint8_t shift = 1;};
struct final_flag{static constexpr uint8_t shift = 2;};

template< class ... Ts >
struct flag_set{
public:
    constexpr operator uint8_t() const { return compute_value(); }
private:
    flag_set() = default;
    constexpr uint8_t compute_value() const {return (uint8_t{0} | ... | (1UL << Ts::shift));}
};
} // namespace details

template<std::size_t Rank, class ... Ts>
struct state_retriever {};

template<std::size_t Rank, class T, class ... Ts>
struct state_retriever<Rank, T, Ts...>{
    template<auto Policy>
    constexpr static auto extract() {
        if constexpr(Policy(T::rank, Rank)){
            return state_retriever<Rank, Ts...>::template extract<Policy>();
        } else {
            return T{};
        }
    }
};

template<std::size_t Rank, class T>
struct state_retriever<Rank, T>{
    template<auto Policy>
    constexpr static auto extract() {
            return T{};
    }
};

struct forwarding_t {
    constexpr static uint8_t flag_set = details::flag_set<>{}; 
};
struct repetition_t {
    constexpr static uint8_t flag_set = details::flag_set<details::repetition_flag>{}; 
};

template<std::size_t Rank, class T>
struct rankify{
    constexpr static std::size_t rank = Rank;
    using state_t = T;
};

template<char Character, class Type = forwarding_t>
struct state_t{
    constexpr static char accepted_input = Character;
    constexpr static uint8_t evaluate(char character){
        auto result = Type::flag_set;
        if(character != Character){result |= details::flag_set<details::failure_flag>{};}
        return result;
    }
};

template<class ... Ts> struct stator_t{};
template<class T, class ... Ts>
struct stator_t< T, Ts...>{
    using state_machine_t = std::variant<T, Ts...>;

private:
    constexpr static bool forward_extraction(std::size_t current_rank, std::size_t target_rank) { return current_rank != target_rank+1; } 
    template<std::size_t Rank, auto Policy>
    constexpr auto retrieve_state() const {
        return state_retriever<Rank, T, Ts...>::template extract<Policy>();
    }

public:
    constexpr void process(char character) const {
        std::cout << "[" << character << "]: ";
        std::visit(
            [](auto state){
                using ranked_state_t = decltype(state);
                using state_t = typename ranked_state_t::state_t;
                std::cout << state_t::accepted_input;},
            state_machine_m
        );
        std::visit( 
            [character, this](auto ranked_state){ 
              using ranked_state_t = decltype(ranked_state);
              using state_t = typename ranked_state_t::state_t;
              constexpr std::size_t rank = ranked_state_t::rank;
              uint8_t flag_set = sizeof...(Ts) != state_machine_m.index() ? details::flag_set<>{} : details::flag_set<details::final_flag>{};
              flag_set |= state_t::evaluate(character);
              switch(flag_set){
              case details::flag_set<>{}:{
                std::cout << "(s)";
                state_machine_m = retrieve_state<rank, &forward_extraction>();
                break;
              }  
              case details::flag_set<details::failure_flag>{}:
              case details::flag_set<details::final_flag, details::failure_flag>{}:{
                std::cout << "(f|ff)";
                state_machine_m = T{};
                break;
              }  
              case details::flag_set<details::final_flag>{}:{
                std::cout << "(fs)";
                break;
              }
              case details::flag_set<details::repetition_flag>{}:{std::cout << "(rs)";break;}
              case details::flag_set<details::repetition_flag, details::failure_flag>{}:{
                std::cout << "(rf)";
                break;
              }  
              case details::flag_set<details::final_flag, details::repetition_flag, details::failure_flag>{}:{
                std::cout << "(frf)";
                break;
              }  
              case details::flag_set<details::final_flag, details::repetition_flag>{}:{
                std::cout << "(frs)";
                break;
              }
              default:break;
              }
            }, 
            state_machine_m
                            );
        std::visit(
            [](auto state){
                using ranked_state_t = decltype(state);
                using state_t = typename ranked_state_t::state_t;
                std::cout << " -> " << state_t::accepted_input<< '\n';},
            state_machine_m
        );
    }

    state_machine_t mutable state_machine_m;
};


#endif /* stator_h */
