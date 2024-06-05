#include"server.hpp"

std::string Server:: colorCode(const std::string& message, int color) {
    std::stringstream ss;
    ss << "\x03" << color << message << "\x03";
    return ss.str();
};

bool isValidChar(char c) {
    return (isalnum(c) || c == '[' || c == ']' || c == '{' || c == '}' ||
            c == '\\' || c == '|' || c == '-' || c == '_');
}

bool Server::validateNickname(std::string nickname)
{
    
    if(nickname.size() > 30)
        return false;
    if(std::isdigit(nickname[0]))
        return false;
   for (size_t i = 0; i < nickname.size(); ++i) {
        char c = nickname[i];
        if (!isValidChar(c)) {
            return false;
        }
    }
    return true;
}

bool Server::prsNickname(std::string nickname,int fd)
{
    for(std::vector<Client>::iterator it = clients.begin();it != clients.end();++it)
    {
        if(nickname.empty())
        {
            std::string pass_err=ERR_ERRONEUSNICKNAME(nickname);
            send(fd,pass_err.c_str(),pass_err.size(),0);
            return false;
        }
        else if(nickname == it->getNickname())
        {
            std::string pass_err=ERR_NICKNAMEINUSE(nickname);
            send(fd,pass_err.c_str(),pass_err.size(),0);
            return false;
        }
        else if(!validateNickname(nickname))
        {
            std::string pass_err=ERR_NICKNAMEINUSE(nickname);
            send(fd,pass_err.c_str(),pass_err.size(),0);
            return false;
        }
        else
            return true;
    }
    return true;
}
