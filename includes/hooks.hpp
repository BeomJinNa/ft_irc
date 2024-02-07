#pragma once
#ifndef HOOKS_HPP
# define HOOKS_HPP

class Message;

void	HookFunctionPass(const Message& message);
void	HookFunctionQuit(const Message& message);
void	HookFunctionUser(const Message& message);
void	HookFunctionNick(const Message& message);
#endif
