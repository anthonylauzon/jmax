%module ftsclient

%{
#include <fts/ftsclient.h>
%}

%include <fts/ftsclient.h>
%include <fts/client/FtsAtom.h>
%include <fts/client/FtsArgs.h>
%include <fts/client/FtsClientException.h>
%include <fts/client/FtsMessageHandler.h>
%include <fts/client/FtsServerConnection.h>
%include <fts/client/FtsSocketConnection.h>
%include <fts/client/FtsObject.h>
