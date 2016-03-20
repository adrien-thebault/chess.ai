/**
*
*  Functions to print colored data in console
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef OUTPUT_H
#define OUTPUT_H

#define INFO(x)    "\x1b[34m[INFO]   \t" x "\x1b[0m\n"
#define WARNING(x) "\x1b[33m[WARNING]\t" x "\x1b[0m\n"
#define SUCCESS(x) "\x1b[32m[SUCCESS]\t" x "\x1b[0m\n"
#define ERROR(x)   "\x1b[31m[ERROR]  \t" x "\x1b[0m\n"

#endif
