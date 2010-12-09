#pragma once
/*
*  Author(s):
*  - Chris  Kilner <ckilner@aldebaran-robotics.com>
*  - Cedric Gestes <gestes@aldebaran-robotics.com>
*
*  Copyright (C) 2010 Aldebaran Robotics
*/


#ifndef _QI_EXCEPTIONS_EXCEPTIONS_HPP_
#define _QI_EXCEPTIONS_EXCEPTIONS_HPP_

#include <exception>
#include <string>

namespace qi {
  namespace transport {

    /// <summary>
    /// Base class for exceptions
    /// </summary>
    class Exception : public std::exception {
    public:

      /// <summary>Default constructor. </summary>
      Exception () {}

      /// <summary>Constructor. </summary>
      /// <param name="message">The message.</param>
      Exception (const std::string & message) { this->message = message; }

      /// <summary>Finaliser. </summary>
      virtual ~Exception () throw () {}

      /// <summary>Gets the exception message. </summary>
      /// <returns>The message</returns>
      virtual const char * what () const throw () { return message.c_str(); }

    private:
      /// <summary> The message </summary>
      std::string message;
    };

    /// <summary>
    /// Exception thrown when creating/opening shared memory segments.
    /// </summary>
    class SharedSegmentInitializationException : public Exception {
    public:
      /// <summary>Default constructor. </summary>
      SharedSegmentInitializationException () {}

      /// <summary>Constructor. </summary>
      /// <param name="message">The message.</param>
      SharedSegmentInitializationException (const std::string & message) : Exception(message) {}

      /// <summary>Finaliser. </summary>
      virtual ~SharedSegmentInitializationException () throw () {}
    };

    /// <summary>
    /// Exception thrown when the connection to the server fails.
    /// </summary>
    class ConnectionException : public Exception {
    public:

      /// <summary>Default constructor. </summary>
      ConnectionException () {}

      /// <summary>Constructor. </summary>
      /// <param name="message">The message.</param>
      ConnectionException (const std::string & message) : Exception(message) {}

      /// <summary>Finaliser. </summary>
      virtual ~ConnectionException () throw () {}
    };

    /// <summary>
    /// Exception thrown when the server encountered a problem.
    /// </summary>
    class ServerException : public Exception {
    public:
      /// <summary>Default constructor. </summary>
      ServerException () {}

      /// <summary>Constructor. </summary>
      /// <param name="message">The message.</param>
      ServerException (const std::string & message) : Exception(message) {}

      /// <summary>Finaliser. </summary>
      virtual ~ServerException () throw () {}
    };

    /// <summary>
    ///Exception thrown when the server encountered a problem when reading.
    /// </summary>
    class ReadException : public Exception {
    public:
      /// <summary>Default constructor. </summary>
      ReadException () {}

      /// <summary>Constructor. </summary>
      /// <param name="message">The message.</param>
      ReadException (const std::string & message) : Exception(message) {}

      /// <summary>Finaliser. </summary>
      virtual ~ReadException () throw () {}
    };

    /// <summary>
    /// Exception thrown when the client encountered a problem when writing.
    /// </summary>
    class WriteException : public Exception {
    public:
      /// <summary>Default constructor. </summary>
      WriteException () {}

      /// <summary>Constructor. </summary>
      /// <param name="message">The message.</param>
      WriteException (const std::string & message) : Exception(message) {}

      /// <summary>Finaliser. </summary>
      virtual ~WriteException () throw () {}
    };

    /// <summary>
    /// Exception thrown when the client can't find a service
    /// </summary>
    class ServiceNotFoundException : public Exception {
    public:
      /// <summary>Default constructor. </summary>
      ServiceNotFoundException () {}

      /// <summary>Constructor. </summary>
      /// <param name="message">The message.</param>
      ServiceNotFoundException (const std::string & message) : Exception(message) {}

      /// <summary>Finaliser. </summary>
      virtual ~ServiceNotFoundException () throw () {}
    };


  }
}

#endif  // _QI_EXCEPTIONS_EXCEPTIONS_HPP_
