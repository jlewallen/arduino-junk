#ifndef HttpRequest_h
#define HttpRequest_h

#define HTTP_MAXIMUM_REQUEST_LENGTH     512
#define HTTP_MAXIMUM_WORK_BUFFER_LENGTH 32

#define HTTP_END_OF_HEADERS             "\r\n\r\n"
#define HTTP_END_OF_HEADERS_LENGTH      4

/**
 * Wrapper class for very basic handling of HTTP requests.
 *
 * class MyWebRequest : public HttpRequest {
 *  public:
 *    MyWebRequest(EthernetClient &c) : HttpRequest(c) { }
 *
 *  public:
 *    virtual void respond(const char *url) {
 *      respondWithJsonMillis();
 *    }
 * };
 *
 * void loop() {
 *   EthernetClient client = server.available();
 *   if (client) {
 *     MyWebRequest active = MyWebRequest(client);
 *     active.handle();
 *   }
 * }
 */
class HttpRequest {
  private:
    char work_buffer[HTTP_MAXIMUM_WORK_BUFFER_LENGTH];
    size_t work_buffer_allocated;
    char *free;
    
  private:
    char buffer[HTTP_MAXIMUM_REQUEST_LENGTH];
    EthernetClient client;
    size_t bytes;
    char *url;

  private:
    char *allocate(size_t length);
    boolean parse();
    
  public:
    HttpRequest(EthernetClient &client);

    /**
     * Returns true if things are still connected.
     */
    boolean connected();

    /**
     * Reads any pending data, parsing the HTTP request and then calls respond
     * when enough has been read. Returns false when the connection is closed.
     */
    boolean tick();

    /**
     * Handles this HTTP request in a blocking, synchronous fashion.
     * Essentially calls tick until false is returned.
     */
    void handle();

  public:
    /**
     * Returns the currently connected EthernetClient.
     */
    EthernetClient &ethernetClient();

    /**
     * Parses the query string for a parameter, returning a buffer with its
     * value. This may fail if the work buffer memory is exhausted, which can
     * be tweaked.
     */
    const char *parameter(const char *name);

  public:

    /**
     * Called when the headers have been parsed and the response can begin. 
     */
    virtual void respond(const char *url) = 0;

    /**
     * Outputs headers to initiate a JSON response.
     */
    virtual void respondWithJsonHeaders();

    /**
     * Outputs a JSON payload with the millis() value. Can be used to test.
     */
    virtual void respondWithJsonMillis();

    /**
     * Outputs a JSON payload with debugging information, mostly memory related.
     * Can be useful to see how close you're getting to the various buffer
     * sizes.
     */
    virtual void respondWithDebugInformation();
};

HttpRequest::HttpRequest(EthernetClient &client) : client(client), bytes(0), url(NULL) {
  memset(buffer, 0, HTTP_MAXIMUM_REQUEST_LENGTH);
  memset(work_buffer, 0, HTTP_MAXIMUM_WORK_BUFFER_LENGTH);
  free = work_buffer;
  work_buffer_allocated = 0;
}

boolean HttpRequest::connected() {
  return client.connected();
}

EthernetClient &HttpRequest::ethernetClient() {
  return client;
}

boolean HttpRequest::tick() {
  boolean finished = false;
  if (client.available()) {
    int16_t c = client.read();
    if (c != -1) {
      if (bytes >= HTTP_MAXIMUM_REQUEST_LENGTH) {
        client.println("Out of memory.");
        finished = true;
      }
      else {
        buffer[bytes++] = c;
        if (bytes >= HTTP_END_OF_HEADERS_LENGTH) {
          if (strncmp(buffer + bytes - HTTP_END_OF_HEADERS_LENGTH, HTTP_END_OF_HEADERS, HTTP_END_OF_HEADERS_LENGTH) == 0) {
            parse();
            respond(url);
            finished = true;
          }
        }
      }
    }
  }
  if (finished || !client.connected()) {
    finished = true;
    delay(1);
    client.stop();
  }
  return !finished;
}

char *HttpRequest::allocate(size_t length) {
  if (work_buffer_allocated + length >= HTTP_MAXIMUM_WORK_BUFFER_LENGTH) {
    client.println("Out of memory.");
    return NULL;
  }
  char *memory = free;
  free += length;
  work_buffer_allocated += length;
  return memory;
}

const char *HttpRequest::parameter(const char *name) {
  const char *p = url;
  const char *start_of_key = NULL;
  const char *start_of_value = NULL;
  size_t name_length = strlen(name);
  while (true) {
    if (*p == '=' && start_of_key != NULL) {
      start_of_value = p + 1;
    }
    else if (*p == 0 || *p == '?' || *p == '&') {
      if (start_of_value == NULL) {
        if (strncmp(p + 1, name, name_length) == 0) {
          start_of_key = p + 1;
        }
      }
      else {
        size_t value_length = (p - start_of_value) + 0;
        char *value = allocate(value_length);
        if (value == NULL) {
          return NULL;
        }
        memcpy(value, start_of_value, value_length);
        value[value_length] = 0;
        return value;
      }
    }
    if (*p == 0) {
      break;
    }
    ++p;
  }
  return NULL;
}

boolean HttpRequest::parse() {
  char *afterVerb = strchr(buffer, ' ');
  if (afterVerb != NULL) {
    char *afterUrl = strchr(afterVerb + 1, ' ');
    if (afterUrl != NULL) {
      url = afterVerb + 1;
      afterUrl[0] = 0;
      return true;
    }
  }
  return false;
}

void HttpRequest::respondWithJsonHeaders() {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println();
}

void HttpRequest::respondWithJsonMillis() {
  respondWithJsonHeaders();
  client.print("{");
  client.print(" \"millis\" : "); client.print(millis()); client.print(" ");
  client.print("}");
  client.println("");
}

void HttpRequest::respondWithDebugInformation() {
  respondWithJsonHeaders();
  client.print("{");
  client.print(" \"bytes\" : "); client.print(bytes); client.print(",");
  client.print(" \"buffer_size\" : "); client.println(HTTP_MAXIMUM_REQUEST_LENGTH); client.print(",");
  client.print(" \"allocated\" : "); client.print(work_buffer_allocated); client.print(",");
  client.print(" \"work_buffer_size\" : "); client.println(HTTP_MAXIMUM_WORK_BUFFER_LENGTH); client.print(" ");
  client.print("}");
  client.println("");
}

void HttpRequest::handle() {
  while (tick()) { }
}

#endif
