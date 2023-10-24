#include "Player.hpp"
#include <iostream>
#include <cassert>
#include <algorithm>

using namespace std;


class SimplePlayer : public Player {
    private:
        string name;
        vector<Card> hand;
    public:
        SimplePlayer(const string &player_name) : name(player_name) {}
        const string &get_name() const override {
            return name;
        }
        void add_card(const Card &c) override{
            hand.push_back(c);
        }

        void helper_trump(int &r2num, int i, Suit &nextsuit) const {
            if((hand.at(i).is_face_or_ace() 
                && hand.at(i).is_trump(nextsuit))){
                r2num++; 
            }
        }
        
        bool make_trump(const Card &upcard, bool is_dealer, int round, 
            Suit &order_up_suit) const override {
            // ROUND 1
            if (round == 1){
                int r1num = 0; 
                for(int i = 0; i < hand.size(); i++){
                    // Trump card that is face/ace  
                    if((hand.at(i).is_face_or_ace() && 
                        hand.at(i).is_trump(upcard.get_suit()))){
                        r1num++; 
                    }
                }
                if(r1num >= 2){
                    order_up_suit = upcard.get_suit();
                    return true; 
                }
                return false; 
            }

            // ROUND 2
            if (round == 2){
                Suit nextsuit = Suit_next(upcard.get_suit());
                if(is_dealer){
                    order_up_suit = nextsuit;
                    return true; 
                }
                else{
                    int r2num = 0; 
                    for(int i = 0; i < hand.size(); i++){
                        // Trump card that is face/ace  
                        helper_trump(r2num, i, nextsuit);
                    }

                    if(r2num >= 1){
                        order_up_suit = Suit_next(upcard.get_suit());
                        return true; 
                    }
                }     
            }
            return false; 
        }
        void add_and_discard(const Card &upcard) override {
                hand.push_back(upcard);
                Card lowestCard = hand[0];
                int lowestIndex = 0;
                for(size_t i = 1; i < hand.size(); ++i) {
                    if(Card_less(hand[i], lowestCard, upcard.get_suit())) {
                        lowestCard = hand[i];
                        lowestIndex = i;
                    }
                }
                hand.erase(hand.begin() + lowestIndex); // remove card at lowestIndex

        }
        Card lead_card(Suit trump) override {
            Card highestCard = Card(TWO, Suit_next(trump)); //guarantees not a trump card
            int count_trump = 0;
            for(size_t i = 0; i < hand.size(); ++i) {
                if(hand.at(i).is_trump(trump)) {
                    count_trump++;
                }
            }
            int removeIndex = -1;
            if(count_trump == hand.size()) { //all cards are trumps, so find highest card 
                for(size_t i = 0; i < hand.size(); ++i) {
                    if(Card_less(highestCard, hand.at(i), trump)) {
                        highestCard = hand.at(i);
                        removeIndex = i;
                    }
                }
                hand.erase(hand.begin() + removeIndex);
                return highestCard;
            }
            else { // at least one non-trump card in hand
                for(size_t i = 0; i < hand.size(); ++i) {
                    if(hand.at(i).is_trump(trump)) { // skip trump cards
                        continue;
                    }
                    if(Card_less(highestCard, hand.at(i), trump)) {
                        highestCard = hand.at(i);
                        removeIndex = i;
                    }
                }
                hand.erase(hand.begin() + removeIndex);
                return highestCard;
            }
        }
        Card play_card(const Card &led_card, Suit trump) override { 
            Card highestCard = Card(TWO, led_card.get_suit());
            Card lowestCard = Card(JACK, trump);
            int removeIndex = -1;
            bool can_follow_suit = false;
            for(size_t i = 0; i < hand.size(); ++i) {
            if(led_card.is_left_bower(trump)) {
            if(hand.at(i).get_suit() == Suit_next(led_card.get_suit()) && 
            !hand.at(i).is_left_bower(trump)) {
            can_follow_suit = true;
            break;
            }
            }
            else if((hand.at(i).get_suit() == led_card.get_suit() && 
            !hand.at(i).is_left_bower(trump)) || 
            (hand.at(i).is_left_bower(trump) 
            && led_card.get_suit() == trump)) {
            can_follow_suit = true;
            break;
            }
            }
            if(can_follow_suit) { 
            for(size_t i = 0; i < hand.size(); ++i) {
            if((led_card.is_left_bower(trump)) && hand.at(i).get_suit() == 
            Suit_next(led_card.get_suit()) && 
            !hand.at(i).is_left_bower(trump) 
            && Card_less(highestCard, hand.at(i), led_card, trump)) {
            highestCard = hand.at(i);
            removeIndex = i;
            }
            else if(((hand.at(i).get_suit() == led_card.get_suit() && 
            !hand.at(i).is_left_bower(trump)) || 
            (hand.at(i).is_left_bower(trump) && 
            led_card.get_suit() == trump)) 
            && Card_less(highestCard, hand.at(i), led_card, trump)){
            highestCard = hand.at(i);
            removeIndex = i;
            }
            }
            hand.erase(hand.begin() + removeIndex);
            return highestCard;
            }
            else {
            for(size_t i = 0; i < hand.size(); ++i) {
            if(Card_less(hand.at(i), lowestCard, trump)) { // lowest card
            lowestCard = hand.at(i);
            removeIndex = i;
            }
            }
            hand.erase(hand.begin() + removeIndex);
            return lowestCard;
            }
        }

        // Maximum number of cards in a player's hand
        static const int MAX_HAND_SIZE = 5;

        // Needed to avoid some compiler errors
        virtual ~SimplePlayer() {}

};

class HumanPlayer : public Player{
    private:
        string name; 
        vector<Card> hand; 
    public:
        HumanPlayer(const std::string &player_name) : name(player_name) {}

        const string &get_name() const override {
            return name;
        }

        void add_card(const Card &c) override {
            hand.push_back(c);
            sort(hand.begin(), hand.end());
        }

        void print_hand() const {
        for (size_t i=0; i < hand.size(); ++i)
            cout << "Human player " << name << "'s hand: "
                << "[" << i << "] " << hand[i] << "\n";
        }

        bool make_trump(const Card &upcard, bool is_dealer, int round, 
            Suit &order_up_suit) const override {
            print_hand();
            cout << "Human player " << name << ", please enter a suit, or \"pass\":\n";
            string decision;
            cin >> decision;

            if (decision != "pass") {
                order_up_suit = string_to_suit(decision);
                return true;
            }
            else {
                return false;
            }
        }

        void add_and_discard(const Card &upcard) override {
            print_hand();
            cout << "Discard upcard: [-1]\n";
            cout << "Human player " << name << ", please select a card to discard:\n";

            int index;
            cin >> index;
            if(index != -1) {
                hand.erase(hand.begin() + index);
                hand.push_back(upcard);
                sort(hand.begin(), hand.end());
            }
        }

        Card lead_card(Suit trump) override {
            print_hand();
            cout << "Human player " << name << ", please select a card:\n";
            int index;
            cin >> index;
            Card lead = hand.at(index);
            hand.erase(hand.begin() + index);
            return lead;
        }

        Card play_card(const Card &led_card, Suit trump) override {
            print_hand();
            cout << "Human player " << name << ", please select a card:\n";
            int index;
            cin >> index;
            Card play = hand.at(index);
            hand.erase(hand.begin() + index);
            return play;
        }
        
        // Maximum number of cards in a player's hand
        static const int MAX_HAND_SIZE = 5;

        // Needed to avoid some compiler errors
        virtual ~HumanPlayer() {}

};

Player * Player_factory(const std::string &name, const std::string &strategy) {
    if (strategy == "Simple") {
        // Create and return a Simple player
        return new SimplePlayer(name);
    } 
    else if (strategy == "Human") {
        // Create and return a Human player
        return new HumanPlayer(name);
    }
    return nullptr;
}

std::ostream & operator<<(std::ostream &os, const Player &p) {
  os << p.get_name();
  return os;
}


