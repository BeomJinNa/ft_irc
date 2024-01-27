#parameters====================================================================

CXX			= c++

COMMONFLAGS	=
CXXFLAGS	= $(COMMONFLAGS) -Wall -Wextra -Werror -std=c++98
LDFLAGS		= $(COMMONFLAGS)

NAME	= ircserv
INCLUDE	= includes/

#sources=======================================================================

SRCS	= srcs/main.cpp \
		  srcs/Command/Command.cpp \
		  srcs/ConstString.cpp \
		  srcs/Server/FixedBufferArray.cpp \
		  srcs/Server/Message/Message.cpp \
		  srcs/Server/Server.cpp \
		  srcs/UserDB/User.cpp \
		  srcs/UserDB/UserDB.cpp \
		  srcs/hooks/JOIN/join.cpp


OBJS	= $(SRCS:.cpp=.o)

TARGET_OBJS = $(OBJS)

#rules=========================================================================

.PHONY: all
all :
	make $(NAME)

$(NAME) : $(TARGET_OBJS)
	$(CXX) -o $@ $(TARGET_OBJS) $(LDFLAGS)

#const options=================================================================

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $< -o $@

.PHONY: clean
clean :
	rm -f $(OBJS)

.PHONY: fclean
fclean :
	make clean
	rm -f $(NAME)

.PHONY: re
re :
	make fclean
	make all
