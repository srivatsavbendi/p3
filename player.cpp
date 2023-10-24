#include "Player.hpp"
#include <iostream>
#include <cassert>
#include <algorithm>

using namespace std;


class SimplePlayer : public Player {
    public:
        string name;
        vector<Card> hand;

        SimplePlayer(const string &player_name) {
            name = player_name;
        }
        const string &get_name() const override {
            return name;
        }
        void add_card(const Card &c) override{
            hand.push_back(c);
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
                        if((hand.at(i).is_face_or_ace() 
                            && hand.at(i).is_trump(nextsuit))){
                            r2num++; 
                        }
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
                if(hand[i].is_trump(trump)) {
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
Card play_card(const Card& led_card, Suit trump) override {
		// play the highest card that follows suit otherwise play the lowest card
		Card highest_followed = hand[0];
		Card lowest_non = hand[0];

		int highest_loc = 0;
		int lowest_loc = 0;

		bool has_follow = false;

		for(int i = 0; i < hand.size(); ++i) {
			Card& card = hand[i];

			if(!has_follow && (card.get_suit(trump) == led_card.get_suit(trump))) {
				has_follow = true;
			}
			
			if(has_follow && 
                card.get_suit(trump) == led_card.get_suit(trump) &&
				(Card_less(highest_followed, card, led_card, trump) ||
				highest_followed.get_suit(trump) != led_card.get_suit(trump))) { 
				highest_followed = card;
				highest_loc = i;
			}

			if(Card_less(card, lowest_non, trump)) {
				lowest_non = card;
				lowest_loc = i;
			}
		}
		if(has_follow) {
			hand.erase(hand.begin() + highest_loc);
			return highest_followed;
		}
		else {
			hand.erase(hand.begin() + lowest_loc);
			return lowest_non;
		}
	}

        // Maximum number of cards in a player's hand
        static const int MAX_HAND_SIZE = 5;

        // Needed to avoid some compiler errors
        virtual ~SimplePlayer() {}

};

class HumanPlayer : public Player{
    public:
        string name; 
        vector<Card> hand; 

        HumanPlayer(const std::string &playername) {
            name = playername;
        }

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



