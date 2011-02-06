freeCGI++ Class Hierarchy v2.4
==============================
http://www.achacha.org/freeCGI/

See VERSION.txt for the release version.
See CONTRIBUTORS.txt for people that contributed to freeCGI++


LEGAL STUFF
===========

The library is provided with a BSD license, 
as is free to change, distribute, and use for profit.

I claim no responsibility for this code.  By using it you are
accepting it as-is and you are assuming full responsibility for it.
That doesn't mean I won't answer question or suggestions.

Support is email based (achacha@hotmail.com or http://www.achacha.org/) and timeliness is 
not guaranteed but I have been doing my best since 1994.


OPEN SOURCE
===========
http://www.freshmeat.net/projects/freecgi/
http://sourceforge.net/projects/freecgi/


After more than 10 years of maintenance, there are very few,
if any, bugs.  The source code is regularly passed through
a lint-and-review process.

If you find bugs, please let me know or if you make fixes
please send them to me so I can add them and credit you.

If you have questions, email them to me, I will try to reply
as time permits (since I do this for fun, it is of lower
priority).

As a base requirement, you should be able to program in C++
to some degree, as the code is very general and specific
functionality may require you to extend the object model.

freeCGI++ is intended as a useful set of classes to get you
started with writing CGIs without having to do it all from
scratch.

Many samples are provided, and  should help you to get started.

The code is very generic C++ with minimal use of STDLIB to make
it usable on a wider range of systems.


FAQ
===
Q: Why don't you use STL containers instead of writting your own?
A: When it was first designed and written, STL was very young and
there weren't any consistent implementations.  Once I decided to
write my own simple container classes, I stuck with it and improved
them as time went on, limiting the complexity of the code and keeping
the interface consistent for people already using it.

Q: I want to compile it for XYZ system, will it work?
A: Since the code is very generic, you should be able to build it on
most compilers that have support for C++ and iostreams.

Q: What's the easiest way to start?
A: Compile variables.cgi and run it, it is a good start to learning
how CGIs work.  Look at the html_help directory also, the class
hierarchy diagram can be very helpful.

Q: How do I write a HelloWorld CGI in C++?
A:
#include <freeCGI.h>
int main()
{
  ACGI acgi;                               // Create the ACGI object

  acgi.mimeHTML();                         // send MIME type
  acgi.htmlStartHTML();                    // send <html>
  acgi.htmlDoHEAD("Hello World Program");  // send <head><title>...</title></head>
  acgi.htmlStartBODY();                    // send <body>

  acgi << "Hello World" << endl;           // send "Hello World"

  acgi.htmlEndBODY();                      // send </body>
  acgi.htmlEndHTML();                      // send </html>

  return 0x0;
}

Q: Will the code compile using a C compiler?
A: No, it is a class hierarchy in C++.


HISTORY
=======

This project was created in 1994 and has gone through some
fixes and updates, the interface and class names have not changed
for over 16 years to provide consistency.  The code was originally
written using Microsoft Visual Studio 1.0.  When written it was the
only C++ class hierarchy that was object oriented.  It was called ACGI++.

In the early 90s, I was working on high speed internet applications but
majority of implementations were in perl.  Perl as a script language
is fine, but it is very slow compared to compiled C/C++ code.  Not
willing to constantly optimize scripts and remove functionality to gain
speed, I wrote this class hierarchy to help deploy CGIs in C++ quickly
and with minimal hassle.  On average code rewritten from perl to C++ 
was 10-12 times faster and allowed more functionality (which was stripped
out when trying to squeeze blood from perl).
As time went on, lots of technologies came and went, but when speed is
most important C/C++ is often the way to go.

In 1995, I ported it to work with Borland C++ 5.0, but that was a futile
attempt as Borland was using their version of STDLIB which was different
enough from Microsoft to force me to use #ifdef's (which I prefer not
to use).

In 1996, I ported the class hierarchy to linux, using Debian Linux 
(version 2.0 or maybe earlier, I forget), added some makefiles and
called it freeCGI++.

In 1997, I ported it to HP-UX 10.x and 11.x (in reality I just recompiled
the code using HP-UX specific flags) and created the open source version of
the code called v2.4.00.  I then rebuilt it under Solaris 2.6 and 2.7,
created makefiles and decided that I don't like writing makefiles for
every system, I'll leave that as "an exercise for the reader."  :)

From 1997+, the hierarchy has gone through little change, mostly minor bug
fixes, lint work, optimization and minor cleanup.



TECHNICAL NOTES
===============

FOR RELEASE BUILDS in *nix:
  Comment out DEBUGOPTIONS= line by inserting a # in front of it
  Add whatever level of optimization you feel is necessary and safe
    (this varies between systems)




Provided makefiles:
freeCGI.sln - Visual C++ 2005 (v8.x)
makefile.linux.gcc - Linux makefile
makefile.cygwin.aCC - HP-UX makefile
makefile.solaris.sunc - Solaris makefile
makefile.hpux.aCC - HP-UX makefile
MSDevStudio60projects.zip - Visual C++ 6.x
MSDevStudio71projects.zip - Visual C++ 7.1


/doc
--Contains helpful documents collected from the web
--CGI standard: RFC1945
--CGI variables and their uses
--URL specifications
--Cookie specification and RFC
--and some others

/htmlhelp
--The on-line help system in HTML
--Just open index.html into your browser

/include
--All the include files necessary.  
  freeCGI.h is the only one that should be included in your C++ code.

/samples/apps
--Some useful app-type samples
/samples/test
--Some MFC samples
/samples/test
--Some debugging and testing samples

/src
--Source code for the C++ objects

Useful Samples
===================
/Apps
-----
variables.vpp
 - Dumps CGI variables, environemnt and allows lookup of environment variables (very useful utility when debugging)
variables_WAP.vpp
 - Similar to variables.cpp, except the output is a WAP compatible XML format
html_ex.cpp 
 - Dynamic HTML generation
cgi_ex.cpp 
 - Form submission variable parser (call from an HTML form)
form_ex.cpp 
 - Shows basic FORM processing (creates a form and then parses submission)
saveIP.cpp
 - Saves the IP of the remote host to a file (useful for saving your IP if you are on a DHCP block)
ShopCart.cpp
 - Basic shopping cart skeleton easy to customize
AMessage.cpp
 - Fully function CGI messageboard/guestbook
ABanner.cpp
 - Basic skeleton for banner rotation (outputs some inlined IMAGE/GIF)
SampleCGI.cpp
 - Simple "Hello World" CGI
multipart_ex.cpp
 - Example CGI to support multi-part submissions

/MFC
----
mfcthrd.cpp
 - MFC multi-threaded example
   Remember to use the MultiThreaded LIB (or DLL) in the project settings.


Some samples for testing functionality
=================================
/test
-----
cookie.cpp
 - HTTP cookie test.
CRC.cpp
 - Using CRC.
crypto.cpp
 - Encryption and encoding.
elems.cpp
 - Continers and element pair lists.
eplist.cpp
 - More element pair lists.
lists.cpp
 - Yet more lists
random.cpp
 - Random number generator sample
rngcrypt.cpp
 - Shows how to use a One-Time Pad for encryption.
state.cpp
 - How to maintain secure state variables.
url.cpp
 - Tests parsing of a URL.
validate.cpp
 - Different routines in freeCGI used for validation.



Alternate UNIX shell script to compile your CGI (assuming g++ compiler)
======================================================================
g++ ~/freeCGI/src/*.cpp ~/freeCGI/samples/apps/$1.cpp -I ~/freeCGI/include -O2 -o $1.cgi 
strip $1.cgi

(
  You can figure the rest out, the code is ANSI compliant and requires little to make
  it run on any other platform that supports ANSI C++
)


Windows compilation (if not using the provided makefiles)
============================================================
1. Create a 32-bit or 64-bit console project
2. Add project search path to {freeCGI path}/include directory
3. Add .cpp filed from either {freeCGI path}/src
5. Add your source file (or a sample)
6. Build and debug

NOTES on older versions of Microsoft Visual C++
 Sometimes a console project is created with LIBC.LIB/LIBCI.LIB missing.
 You will get LNK2001 error for iostream objects.  To fix it just add
 LIBCD.LIB and LIBCID.LIB to debug project library list and
 LIBC.LIB and LIBCI.LIB to the release library.



CHANGE HISTORY:
===============
2.4.10
  Upgraded the Microsoft solution files to v10.0 (MSDev 2010)
  Added cygwin makefile (which is really the generic linux gcc but people kept asking)
  Fixed cookie.cpp test file that demostrates usage of RFC-2019 and Netscape type cookies
  Some bug fixes submitted by Auron Priestley
  Cleaned up examples and added some more comments
2.4.09
  Removed SCO makefiles, not going support SCO if they rely of frivolous lawsuits and attack Linux
  Added AHTMLHelper class to help with formatting some data (mostly static helper methods)
  Cleaned up and highly improved multipart_ex.cpp to display more output about the multipart submitions and data processed
2.4.08
  Enhanced variables.cgi to display nice results and allow query of environment
  Added extra tracing code if built with _DEBUG_DUMP_ and _FULL_DEBUG_
  Fixed parsing issues with post submits and IIS 4.0
  Added routines for string concatenation
  Spell checked the documentation (some typos snuck in)
  Made all tags HTML4.0/XML compliant (singular tags are now <br/> instead of <br>)
2.4.07
  Cleaned up memory usage
  Enhanced error handling
  Fixed MSVC 7.0 sln file
2.4.06
  Memory leak fixed
  Some minor enhancements
  Ported to MSVC++ 7.0
  Fixed usage of <sstream> usage instead of deprecated <strstream>
2.4.05
  Minor bug fixes and some optimization
2.4.04
  Added support for multiple cookies in response HTTP header
2.4.03
  List remove bug fix, thanks to John Loemban Tobing
  Cleaned up and documented AMessage sample
  Added support for arbitrary mime type
  Added samples/Apps for variable dumps in HTML and WAP based WML
  \r\n is consistent with HTTP RFC
2.4.02
  Ported to SCO Unix thanks to Allan Clark of SCO
  (given the litigious nature of SCO I may remove support)
2.4.01
  Ported to SunC 5.0 and templated stream classes
2.4.00
  Ported from ACGI++ and ported to HPUX 10.x/11.x
