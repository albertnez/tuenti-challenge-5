/*
 * For this problem, we were given a code in Glagol. The code used the
 * algorithm RANDU, which is discouraged for using random numbers. 
 *
 * https://en.wikipedia.org/wiki/RANDU
 *
 * The RANDU sequence is the follwing: Xi+1 = Xi*65539 mod 2^31. The code also 
 * call Randomize, which uses the time for seed. Although we do not know the
 * start time of playing, we know that each round happens exactly after 2 
 * minutes, and as the time in Randomize uses up to miliseconds, we know that
 * if the first seed is X, the seed of the next play is X+120000. As the machine
 * reboots everyday, there are only 3600*24*1000 possible starting seeds.
 * Given the cards of the four first rounds, we can see if we can get a unique
 * combination of these cards in seeds X+120000*i.
 *
 * Here I am assuming that we will play if we find that the sequence of cards
 * only happens in one starting seed, and we can win in a round, not necessary
 * the 5th one, as one could bet more in further rounds knowing he is going
 * to win, and bet minimum money or just wait otherwise.
 *
 *
 * As a curiosity, I copied all the copied in the image by hand, and then I
 * found this url of a Russian forum where the creators were discussing how 
 * to present the code: 
 * 
 * http://forum.ru-board.com/topic.cgi?forum=29&topic=15006
 *
 * Where one could copy and paste the code from there. But there was any
 * hint of how to solve the problem unfortunately.
 *
 * For N = 100, total time is ~40 seconds.
 */
#include <iostream>
#include <vector>
#include <sstream>
#include <queue>

typedef std::pair<unsigned,unsigned> Pair;
struct State {
  unsigned seed;
  unsigned play;
  unsigned round;
  State(unsigned seed, unsigned play, unsigned round) 
      : seed(seed), play(play), round(round) {}
  State() {}
};

const unsigned OFFSET = 120000; // Add two minutes

// Get the next number from RANDU
inline unsigned long long next(unsigned long long x) {
  return (x*65539)%(1ULL<<31);
}

// AUX IO Operations
std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& v) {
  os << "(";
  for (const std::string& s : v) {
    os << s << ", ";
  }
  return os << ")";
}

std::ostream& operator<<(std::ostream& os, const std::vector<unsigned>& v) {
  for (unsigned x : v) {
    os << x << " ";
  }
  return os << std::endl;
}

// Shuffle given deck with the given seed. From the algorithm in the image
void shuffle(std::vector<std::string>& deck, unsigned long long seed) {
  for (unsigned i = 0; i < 52; ++i) {
    unsigned rand = seed%52;
    std::swap(deck[i], deck[rand]);
    seed = next(seed);
  }
}

// Shufle given deck of ids, from the algorithm in the image
void shuffle(std::vector<unsigned>& deck, unsigned long long seed) {
  for (unsigned i = 0; i < 52; ++i) {
    unsigned rand = seed%52;
    std::swap(deck[i], deck[rand]);
    seed = next(seed);
  }
}

// From card name to id [0, 51]
unsigned nameToId(const std::string& name) {
  // S H D C
  int id = 0;
  std::istringstream ss(name.substr(1));
  ss >> id;
  --id;
  if (name[0] == 'H') id += 13;
  else if (name[0] == 'D') id += 26;
  else if (name[0] == 'C') id += 39;
  return id;
}

// From id [0, 51] to name
std::string idToName(unsigned id) {
  const std::vector<std::string> suits {"S", "H", "D", "C"};
  std::string name = suits[id/13] + std::to_string((id%13)+1);
  return name;
}

// Score of a card, giving Aces a score of 11
unsigned score(unsigned id) {
  unsigned num = (id%13)+1;
  if (num == 1) return 11;
  if (num > 10) return 10;
  return num;
}

// True if we win by taking N cards
bool winsTakingNCards(unsigned n, const std::vector<unsigned>& deck) {
  unsigned ownAces = 0, ownScore = 0, ownCards = 0;
  unsigned dealerAces = 0, dealerScore = 0;

  unsigned ind = 0;
  bool dealerPlaying = true, ownPlaying = true;
  while (dealerPlaying || ownPlaying) {
    // Our move
    if (ownPlaying) {
      ++ownCards;
      ownAces += (score(deck[ind]) == 11);
      ownScore += score(deck[ind]);
      ++ind;
      if (ownScore > 21 && ownAces > 0) {
        --ownAces;
        ownScore -= 10;
      }
      if (ownCards >= n) ownPlaying = false;
    }
    // Dealer move
    if (dealerPlaying) {
      dealerAces += (score(deck[ind]) == 11);
      dealerScore += score(deck[ind]);
      ++ind;
      if (dealerScore > 21 && dealerAces > 0) {
        --dealerAces;
        dealerScore -= 10;
      }
      dealerPlaying = (dealerScore < 17);
    }
  }
  return (ownScore <= 21 && (ownScore > dealerScore || dealerScore > 21));
}

// True if the player can win the dealer with the given deck
bool isWinningPlay(const std::vector<unsigned>& deck) {
  if (score(deck[1]) + score(deck[3]) == 21) return false; // Dealer blackjack
  if (score(deck[0]) + score(deck[2]) == 21) return true; // our blackjack

  // Try by taking 3 to 11 cards.
  for (unsigned n = 3; n <= 11; ++n) {
    if (winsTakingNCards(n, deck)) return true;
  }
  // No way to win
  return false;
}

// Returns The vector of 4 rounds of each play. That is, data[0] contains
// a vector of 4 elements, each representing the two cards the player receives.
std::vector<std::vector<Pair>> readData() {
  unsigned P;
  std::cin >> P;
  std::vector<std::vector<Pair>> data(P, std::vector<Pair>(4));
  for (unsigned i = 0; i < P; ++i) {
    std::string line;
    std::cin >> line;
    std::string::size_type pos = 0;
    for (unsigned j = 0; j < 4; ++j) {
      std::string::size_type commaPos = line.find(',', pos);
      std::string::size_type limitCard = pos+2;
      if (line[limitCard] >= '0' && line[limitCard] <= '9') {
        ++limitCard;
      }
      data[i][j] = Pair(nameToId(line.substr(pos, limitCard - pos)), 
          nameToId(line.substr(limitCard, commaPos - limitCard)));
      pos = commaPos + 1;
    }
  }
  return data;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  std::vector<std::vector<Pair>> plays = readData();


  const unsigned maxRound = 3;

  std::vector<bool> playWithoutSolutionYet(plays.size(), true);
  std::vector<bool> playDiscarded(plays.size(), false);
  std::vector<std::string> playSolutions(plays.size(), "WITHDRAW");
  
  std::vector<unsigned> deck(52);
  std::vector<unsigned> auxDeck(52);
  unsigned bestRound = 0;
  std::queue<State> q;
  // Iterate over all seeds. If for one seed the cards coincide, push the next
  // round in a queue indicating that the seed is after 2 minutes.
  for (unsigned seed = 1; seed < 3600*24*1000; ++seed) {
    // Shuffle with seed
    for (unsigned i = 0; i < 52; ++i) deck[i] = i;
    shuffle(deck, seed);
    
    // Check the initial play
    for (unsigned i = 0; i < plays.size(); ++i) {
      if (!playDiscarded[i]) { // No need to keep looking if discarded
        if (plays[i][0].first == deck[0] && plays[i][0].second == deck[2]) {
          // Coincides first round, push next round for seed + offset.
          q.push(State(seed+OFFSET, i, 1));
        }
      }
    }
    // Check the waiting
    while (!q.empty() && q.front().seed == seed) {
      State u = q.front();
      q.pop();
      unsigned p = u.play;
      unsigned r = u.round;
      if (plays[p][r].first == deck[0] && plays[p][r].second == deck[2]) {
          bestRound = std::max(bestRound, r+1);
        if (r == maxRound) {
          if (playWithoutSolutionYet[p]) {
            // If it can win any round, wins
            bool canWin = false;
            // number of wins
            Pair winCards;
            // try for any of the next rounds in the day
            for (unsigned num = 0; !canWin && num < 3600*24; ++num) {
              for (unsigned i = 0; i < auxDeck.size(); ++i) auxDeck[i] = i;
              shuffle(auxDeck, seed + OFFSET*(num+1));
              if (num == 0) winCards = Pair(auxDeck[0], auxDeck[2]);

              bool wins = isWinningPlay(auxDeck);
              if (wins) canWin = true;
            }
            if (canWin) {
              // Wins, get the cards
              playSolutions[p] = idToName(winCards.first) + idToName(winCards.second);
              playWithoutSolutionYet[p] = false;
            }
            else {
              // No win possible, discard
              playWithoutSolutionYet[p] = false;
              playDiscarded[p] = true;
            }
          }
          else {
            // More than one solution, discard
            playSolutions[p] = "WITHDRAW";
            playDiscarded[p] = true;
          }
        }
        else {
          // coincides, again push the next round
          q.push(State(seed+OFFSET, p, r+1));
        }
      }
    }
  }

  // Print solutions
  for (const std::string& solution : playSolutions) {
    std::cout << solution << std::endl;
  }
}

