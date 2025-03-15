1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

    > **Answer**: The remote client use a delimiter to signify the end of the command output from the server. Looping over the stream of input from the server until the delimiter is read ensures that the entirety of the command output is read before performing operations on it. Using fixed size chunks of data in transmission allows the client to wait until it has received the entire chunk of data.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

    > **Answer**: A networked shell protocol would have to rely on delimiters within the stream to alert the developer to the message boundaries. If this issue were not handled properly the client and or server could end up using partial reads to execute commands and could lead to undefined behavior.

3. Describe the general differences between stateful and stateless protocols.

    > **Answer**: Stateful protocols rely on a client-server architecture that has the server saving session information about the connection whereas stateless protocols do not save session information and simply transmit data between the client and server.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

    > **Answer**: There are instances where a program would want messages to send quickly between applications without having to wait for a confirmation response back. UDP protocol provides the ability to send data quickly at the cost of possibly loosing the data and not knowing that the data was lossed.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

    > **Answer**: Network sockets are the abstraction provided by the operating system to allow for network communications.