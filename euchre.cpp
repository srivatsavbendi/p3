#include <iostream> 
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <string>
#include <array> 
#include <vector>
#include <sstream>
#include "Card.hpp"
#include "Pack.hpp"
#include "Player.hpp"


using namespace std;


class Game {  // WRITE CTOR AND DTOR 
  private:
    ifstream& instream; 
    string shuffled; 
    int points; 
    std::vector<Player*> players;
    Pack pack;
    int dealer; 
    int hand;
    int round;
    Card upcard; 
    Suit trump; 
    int leader; 
    int currentplayer;  // player to the left gets first cards
    int team1tricks;
    int team2tricks; 
    int team1score; 
    int team2score;
    int teamthatorderedup; 
    Card led; 

  public:
    Game(ifstream& in, string shuffleornot, int num, vector<Player*> playerset)
      : instream(in), shuffled(shuffleornot), points(num), players(playerset){

      pack = Pack(instream); 
      dealer = 0;
      leader = dealer + 1; 
      currentplayer = 1;
      hand = 0; 
      round = 1;
      team1tricks = 0; 
      team2tricks = 0; 
      team1score = 0; 
      team2score = 0; 
      trump = SPADES; 
    }
    std::vector<Player*> getplayers(){
      return players; 
    }

    void shuffle(){
      pack.shuffle(); 
    }

    void deal(){
      int numcardsdealt[8] = {3, 2, 3, 2, 2, 3, 2, 3};
      if(dealer > 3){
        dealer = 0; 
      } 

      currentplayer = dealer + 1; 
      for(int i = 0; i < 8; i++){
        if(currentplayer > 3){
          currentplayer = 0; 
        } 

        for(int x = 0; x < numcardsdealt[i]; x++){
          players.at(currentplayer)->add_card(pack.deal_one()); 
        }

        currentplayer++; 
      }
    }

    void make_trump(){
      int currentplayer = dealer + 1; 
      bool orderedup = false;

      for(int i = 0; i < players.size() ; i++){
        if(currentplayer > 3){
          currentplayer = 0; 
        } 
        
        if (currentplayer == dealer){ 
          orderedup = players.at(currentplayer)->make_trump(upcard, true, round, trump);
        }
        else{
          orderedup = players.at(currentplayer)->make_trump(upcard, false, round, trump);
        }


        if(orderedup == false){
          cout << players.at(currentplayer)->get_name() << " passes" << endl;  
        }
        if(orderedup == true){
          cout << players.at(currentplayer)->get_name() << " orders up " << trump << endl; 
          players.at(dealer)->add_and_discard(upcard);   
          if(currentplayer == 0 || currentplayer == 2){
            teamthatorderedup = 1; 
          }
          if(currentplayer == 1 || currentplayer == 3){
            teamthatorderedup = 2; 
          }
          break;  
        }
        
        currentplayer++; 
      }

      if(orderedup == false){
        round = 2; 
        for(int i = 0; i < players.size(); i++){
          if(currentplayer > 3){
            currentplayer = 0; 
          } 
          
          if (currentplayer == dealer){ 
            orderedup = players.at(currentplayer)->make_trump(upcard, true, round, trump);
          }
          else{
            orderedup = players.at(currentplayer)->make_trump(upcard, false, round, trump);
          }


          if(orderedup == false){
            cout << players.at(currentplayer)->get_name() << " passes" << endl;  
          }
          if(orderedup == true){
            cout << players.at(currentplayer)->get_name() << " orders up " << trump << endl;
            if(currentplayer == 0 || currentplayer == 2){
            teamthatorderedup = 1; 
            }
            if(currentplayer == 1 || currentplayer == 3){
              teamthatorderedup = 2; 
            }
            break; 
          }
          
          currentplayer++; 
        }
      }
    }
    
    int play_trick(){
      Card highestcard; 
      int trickwinner = -1; 

      currentplayer = leader;
      if(currentplayer > 3){
        currentplayer = 0; 
      } 
      if(dealer > 3){
        dealer = 0; 
      } 
      if(leader > 3){
        leader = 0; 
      } 
      
      led = players.at(leader)->lead_card(trump); 
      cout << led << " led by " << players.at(leader)->get_name() << endl;  
      highestcard = led;
      trickwinner = currentplayer; 

      currentplayer++; 

      for(int i = 0; i < players.size()-1; i++){
        if(currentplayer > 3){
          currentplayer = 0; 
        } 
        Card playedcard = players.at(currentplayer)->play_card(led, trump);
        cout << playedcard << " played by " << players.at(currentplayer)->get_name() << endl;
        if(Card_less(highestcard, playedcard, led, trump)){
          highestcard = playedcard;
          trickwinner = currentplayer; 
        }
        currentplayer++; 
      }
      return trickwinner; 
      
    }

    void play_hand(){
      if(leader > 3){
        leader = 0; 
      }
      if(dealer > 3){
        dealer = 0; 
      }
      if(shuffled == "shuffle"){
        shuffle(); 
      }
      deal(); 
      cout << "Hand " << hand << endl;  
      if(leader > 3){
          leader = 0; 
        }
      if(dealer > 3){
        dealer = 0; 
      }
      cout << players.at(dealer)->get_name() << " deals" << endl;  

      upcard = pack.deal_one();
      cout << upcard << " turned up" << endl;   
      cout << endl; 

      make_trump(); 
      cout << endl; 
      
      for(int i = 0; i < 5; i++){
        int trickwinner = play_trick(); 
        cout << players.at(trickwinner)->get_name() << " takes the trick" << endl; 
        cout << endl; 
        leader = trickwinner;

        if(trickwinner == 0 || trickwinner == 2){
          team1tricks++;
        }
        if(trickwinner == 1 || trickwinner == 3){
          team2tricks++;
        }

        // reset 
        round = 1; 
        // cout << "hand " << hand << " round " << round << " leader " << leader << " currentplayer " << currentplayer << " team1tricks " << team1tricks << " team2tricks " << team2tricks << endl; 
      }
      //cout << team1tricks << endl; 
      //cout << team2tricks << endl; 


      bool march = false; 
      bool euchred = false; 

      if(teamthatorderedup == 1){
        if(team1tricks == 3 || team1tricks == 4){
          team1score = team1score + 1; 
        }
        if(team1tricks == 5){   // MARCH
          team1score = team1score + 2; 
          march = true; 
        }
        if(team2tricks == 3 || team2tricks == 4 || team2tricks == 5){   // EUCHRED
          team2score = team2score + 2; 
          euchred = true; 
        }
      }
      else if(teamthatorderedup == 2){
        if(team2tricks == 3 || team2tricks == 4){
          team2score = team2score + 1; 
        }
        if(team2tricks == 5){   // MARCH
          team2score = team2score + 2; 
          march = true;
        }
        if(team1tricks == 3 || team1tricks == 4 || team1tricks == 5){   // EUCHRED
          team1score = team1score + 2; 
          euchred = true; 
        }
      }

      if(team1tricks > team2tricks){ // TEAM 1 WINS
        cout << players.at(0)->get_name() << " and " << players.at(2)->get_name() << " win the hand" << endl; 
      }
      else if(team2tricks > team1tricks){ // TEAM 2 WINS
        cout << players.at(1)->get_name() << " and " << players.at(3)->get_name() << " win the hand" << endl; 
      }
      else{
        // WHAT TO DO WHEN THEY ARE EQUAL?
      }

      if(march){
        cout << "march!" << endl; 
      }
      if(euchred){
        cout << "euchred!" << endl; 
      }

      cout << players.at(0)->get_name() << " and " << players.at(2)->get_name() << " have " << team1score << " points" << endl; 
      cout << players.at(1)->get_name() << " and " << players.at(3)->get_name() << " have " << team2score << " points" << endl;
      team1tricks = 0; 
      team2tricks = 0; 
 
    }
    
    void play(){
      while(team1score < points && team2score < points){
        play_hand(); 
        hand++; 
        dealer++;
        if(dealer > 3){
          dealer = 0; 
        }
        leader = dealer+1; 
        if(leader > 3){
        leader = 0; 
        }
        round = 1; 
        pack.reset();  
        cout << endl; 
        
      }
      if(team1score > team2score){
        cout << players.at(0)->get_name() << " and " << players.at(2)->get_name() << " win"; 
      }else if(team2score > team1score){
        cout << players.at(1)->get_name() << " and " << players.at(3)->get_name() << " win"; 
      }
    }
};

int main(int argc, char* argv[]) {
  string filein = argv[1]; 
  string shuffle = argv[2]; ; 
  int points = atoi(argv[3]); 
  string name0 = argv[4];  
  string type0 = argv[5]; 
  string name1 = argv[6]; 
  string type1 = argv[7];  
  string name2 = argv[8];  
  string type2 = argv[9];   
  string name3 = argv[10]; 
  string type3 = argv[11];

  ifstream instream; 
  instream.open(filein);
  if (!instream.is_open()) {
    cout << "Error opening " << filein << endl;
      return 1; 
  }  

  Player* one = Player_factory(name0, type0);
  Player* two = Player_factory(name1, type1);
  Player* three = Player_factory(name2, type2);
  Player* four = Player_factory(name3, type3);

  vector<Player*> playerset; 
  playerset.push_back(one); 
  playerset.push_back(two); 
  playerset.push_back(three); 
  playerset.push_back(four); 

  Game game(instream, shuffle, points, playerset);
  game.play();
  // game.deal() CHECKED TO MAKE SURE ALL 4 PLAYERS GOT 5 CARDS


  for (size_t i = 0; i < playerset.size(); ++i) {
     delete playerset[i];
  }

}
