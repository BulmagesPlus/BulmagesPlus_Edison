#ifndef PDEFS_PLUGINBF_CLIENTE_H
#define PDEFS_PLUGINBF_CLIENTE_H


#ifdef Q_OS_WIN32

#ifdef PLUGINBF_CLIENTE
#define PLUGINBF_CLIENTE_EXPORT __declspec(dllexport)
#else
#define PLUGINBF_CLIENTE_EXPORT __declspec(dllimport)
#endif

#else

#define PLUGINBF_CLIENTE_EXPORT

#endif


#endif

