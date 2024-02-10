#pragma once
#ifndef HOOKS_HPP
# define HOOKS_HPP

class Message;

void	HookFunctionQuit(const Message& message);
void	HookFunctionPass(const Message& message);
void	HookFunctionUser(const Message& message);
void	HookFunctionNick(const Message& message);
void	HookFunctionJoin(const Message& message);
void	HookFunctionKick(const Message& message);
// void	HookFunctionMode(const Message& message);
void	HookFunctionTopic(const Message& message);
void	HookFunctionPing(const Message& message);
void	HookFunctionPart(const Message& message);
void	HookFunctionInvite(const Message& message);
void	HookFunctionConnect(const Message& message);
#endif
