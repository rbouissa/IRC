#define ERR_PASSWDMISMATCH()  (" ERROR 464 :Password incorrect\r\n")
#define ERR_NEEDMOREPARAMS(cmd) (": 461 " + cmd + " :Not enough parameters\r\n")
#define ERR_NICKNAMEINUSE(nick) (nick + " Error 433 :Nickname is already in use\r\n")
#define ERR_NONICKNAMEGIVEN() (" ERROR 431 :No nickname given\r\n")
#define ERR_NOSUCHNICK(nick) (nick + " 401 :No such nick/channel\r\n")
#define ERR_NOSUCHCHANNEL(channel) (": 403 " + channel + " :No such channel\r\n")
#define RPL_ENDOFWHOIS(nickname) (nickname + " 318 :End of /WHOIS list\r\n")