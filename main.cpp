// Evil Hangman Plus Plus
// By Joel Savitz

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>

#include <exception>

std::shared_ptr<std::vector<std::string>> loadDictionary() ;

void playGame(std::vector<std::string>& vectorDictionary) ;

unsigned int askUserForWordLength() ;
unsigned int askUserForMaxGuesses() ;
bool askUserIfTheyWantToCheat() ;
bool askIfUserWantsToPlayAgain() ;
char askUserForGuess() ;

bool checkGuess(char currentGuess, std::vector<std::string>& vectorDictionaryPartition, std::string& stringGameKey, bool userWantsToCheat) ;

template <class T>
struct Prompt_generic {
protected:

  T _input ;

public:

  Prompt_generic() {} ;

  Prompt_generic(char const * prompt) {
    std::cout << prompt ;
    std::cin >>  _input ;
  }
  

  T& operator * () {
    return _input ;
  }

} ; // struct Prompt_generic

template <class T> 
struct Prompt : public Prompt_generic<T> {} ;

template <>
struct Prompt<bool> : public Prompt_generic<bool> {

  Prompt(char const * prompt) {
    std::cout << prompt << "(Y/N) " ;
    std::string tempInput ;
    bool secondAsk = false ;

    do {
      if ( secondAsk ) std::cout << "Invalid input. Try again" << std::endl 
        << prompt << "(Y/N) " ;
      std::cin >> tempInput;
      secondAsk = true ;
      //std::cout << tempInput << " is what you answered" ;
    } while ( 
        tempInput.find("Y") == std::string::npos && 
        tempInput.find("y") == std::string::npos &&
        tempInput.find("N") == std::string::npos &&
        tempInput.find("n") == std::string::npos ) ;
  
    switch(*tempInput.c_str()) {
      case 'Y':
      case 'y':
        _input = true ;
        break ;
      case 'N' :
      case 'n' :
      default:
        _input = false ;
        break ;
    }
  }

} ;

template <>
struct Prompt<unsigned int> : public Prompt_generic<unsigned int> {

  Prompt(char const * prompt) {
    std::cout << prompt << "(int > 0) " ;
    std::string tempInput ;
    bool canConvertToInt = false ;
    
    while ( ! canConvertToInt) {
      std::cin >> tempInput ;
      try {
        _input = std::stoi(tempInput) ;
        canConvertToInt = true ;
      } catch ( ... ) {
        std::cout << "Invalid input. Try again." << std::endl
         << prompt << "(int > 0)" ;
      }
    }
  }
} ;

template <>
struct Prompt<char> : public Prompt_generic<char> {
  
  Prompt(char const * prompt) {

    std::cout << prompt << "(abc..)" ;
    std::string tempInput ;
    std::cin >> tempInput ;
    _input = tempInput.at(0) ;
  }

} ;

int main() {

  try {

    auto pVectorDictionary = loadDictionary() ;

    std::vector<std::string>& vectorDictionary = *pVectorDictionary ;
    
    playGame(vectorDictionary) ;
    

  } catch ( char const * strMessage) {
    std::cerr << strMessage << std::endl ;
    return 1 ; 
  }


  return 0 ;
}

std::shared_ptr<std::vector<std::string>> loadDictionary() {
  
  std::ifstream fileDictionary("dictionary.txt", std::ios::in) ;
  std::string line ;


  auto vectorDictionary = std::make_shared<std::vector<std::string>>() ;
  
  // Save content of file seperated by '\r' to vector, one unit at a time
  if (fileDictionary.is_open()) {
    while ( getline(fileDictionary, line)) {
      vectorDictionary->push_back(line) ;
    }
  }
  else {
    // If the file does not exist, the entire program must be halted
    throw "There was an error reading the local file dictionary.txt" ;
  }

  fileDictionary.close() ;

  return vectorDictionary ;
}

void playGame(std::vector<std::string>& vectorDictionary) {
  bool userWantsToPlay = true , userWantsToCheat = false, userWins = false ; 
  int wordLength = -1, guessesLeft = -1 ;

  std::vector<std::string> vectorDictionaryPartition ;
  std::string stringGameKey ;

  std::set<char> guessedLetters ;

  char currentGuess ;


  // The first time they play, I assume that this contition should default to true
  while ( userWantsToPlay ) {
    // TODO: validate word length to words that exist
    wordLength = askUserForWordLength() ;
    guessesLeft = askUserForMaxGuesses() ;
    userWantsToCheat = askUserIfTheyWantToCheat() ;

    // Knowing how long the word is allows us to construct the first blank guess key
    stringGameKey.clear() ;
    for ( int i = 0 ; i < wordLength; i++) {
      stringGameKey.push_back('-') ;
    }

    vectorDictionaryPartition.clear() ;
    for(auto word : vectorDictionary) {
      if (word.size() == wordLength ) {
        std::cout << word << std::endl ;
        vectorDictionaryPartition.push_back(word) ;
      }
    }

    guessedLetters.clear() ;
    userWins = false ;
    
    while ( guessesLeft > 0 ) {

      // Display current status TODO

      std::cout << "Current key: " << stringGameKey << " Guesses left: " << guessesLeft << std::endl ;
      std::cout << "Guesses letters:" ;
      for (auto letter : guessedLetters) {
        std::cout << letter << ", " ;
      }
      std::cout << std::endl ;

      do {
        currentGuess = askUserForGuess() ;
      } while ( guessedLetters.find(currentGuess) != guessedLetters.end() ) ;

      guessedLetters.insert(currentGuess) ;
      if (checkGuess(currentGuess, vectorDictionaryPartition, stringGameKey, userWantsToCheat) ) {
        if (stringGameKey.find("-") == std::string::npos ) {
          userWins = true ;
          break ;
        }
      } 
      else {
        guessesLeft-- ;
      }
    }

    if ( userWins ) {
      std::cout << "You win! Impossible!" << std::endl ;
    }
    else {
      std::cout << "You lose!" << std::endl ;
    }

    userWantsToPlay = askIfUserWantsToPlayAgain() ;
  }
}

template <typename T>
T promptUser(const char * promptString) {
  return **std::make_shared<Prompt<T>>(promptString) ;
}

unsigned int askUserForWordLength() {
  return promptUser<unsigned int>("How long should the mystery word be? ") ;
}

unsigned int askUserForMaxGuesses() {
  return promptUser<unsigned int>("How many guesses would you like to have? ") ;
}  

bool askUserIfTheyWantToCheat() {
  return promptUser<bool>("Do you want to cheat? ") ;
}

bool askIfUserWantsToPlayAgain() {
  return promptUser<bool>("Do you want to play again? ") ;
}

char askUserForGuess() {
  return promptUser<char>("Guess a letter ") ;
}

bool checkGuess(char currentGuess, std::vector<std::string>& vectorDictionaryPartition, std::string& stringGameKey, bool userWantsToCheat) {
  std::map<std::string,std::vector<std::string>> mapDictionaryPartitions ;

  std::string tempKey ;

  for(auto word : vectorDictionaryPartition) {
    tempKey.clear() ;
    for (int i = 0 ; i < word.size(); i++) {
      if (word[i] == stringGameKey[i] || word[i] == currentGuess) {
        tempKey.push_back(word[i]) ;
      }
      else {
        tempKey.push_back('-') ;
      }
    }
    mapDictionaryPartitions[tempKey].push_back(word) ;
  }

  int maxPartitionSize = 0 ;
  std::string* ptrMaxPartitionKey = nullptr ;
  std::vector<std::string>* ptrMaxPartition = nullptr ;


  for(auto it = mapDictionaryPartitions.begin(); it != mapDictionaryPartitions.end(); it++)
  {
    if (userWantsToCheat) {
      std::cout << (*it).first << " " << (*it).second.size() << std::endl ;
    }
    if ((*it).second.size() > maxPartitionSize) {
      maxPartitionSize = (*it).second.size() ;
      ptrMaxPartitionKey = const_cast<std::string*> (&(*it).first) ;
      ptrMaxPartition = &(*it).second ;
    }
  }

  stringGameKey = *ptrMaxPartitionKey ;
  vectorDictionaryPartition = *ptrMaxPartition ;

  if (stringGameKey.find(currentGuess) == std::string::npos) {
    std::cout << "Incorrect guess." << std::endl ;
    return false ;
  }
  else {
    std::cout << "You guessed correctly!" << std::endl ;
    return true ;
  }
  
}
