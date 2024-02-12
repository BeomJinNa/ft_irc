#pragma once
#ifndef HOOKS_HPP
# define HOOKS_HPP

class Message;

void	HookFunctionQuit(const Message& message);
void	HookFunctionPass(const Message& message);
void	HookFunctionUser(const Message& message);
void	HookFunctionNick(const Message& message);
void	HookFunctionPrivmsg(const Message& message);
#endif
