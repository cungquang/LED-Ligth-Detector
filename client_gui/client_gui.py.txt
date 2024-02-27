####################################################
#   OS: Linux
#   SETUP:
#       sudo apt-get install python3 python3-tk
#	sudo apt install python3-pip
#       pip3 install matplotlib
#   RUN:
#       python3 client_gui.py
####################################################

import socket
import _thread
from tkinter import *
from tkinter import ttk
from matplotlib.figure import Figure 
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

#Command
SEND_BUTTON_TEXTS = ["help", "?", "count", "length", "dips", "history", "stop"]

#Port & IP of the server
UDP_SERVER_IP = "192.168.7.2"
UDP_SERVER_PORT = 12345
sock = 0
data = []

########################
# HELPER FUNCTION
########################

def isFloat(text_message):
    try:
        float(text_message)
        return True
    except ValueError:
        return False

def readTextToData(text_message):
    text_message = text_message.replace(" ", ",")
    text_message = text_message.replace("\n", ",")
    text_message = text_message.replace("\r", ",")
    text_message = text_message.strip(", \n")
    arr_data = text_message.split(",")
    
    #Convert float data into array -> update data (global)
    data = [float(x) for x in arr_data if isFloat(x)]
    return data


########################
# UDP SOCKET 
########################

def openSocketPort():
    global sock
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def refresh_ui():        
    global data

    #Plot the data if is not empty 
    if data:
        plot_data(data)
    
    #Clear data after read
    data = []       

def sendUdpMessage(msg):
    global sock, readText
    print("Command message:\n", msg)

    #Initial value for readText
    readText.set("")
    sock.sendto(msg.encode(), (UDP_SERVER_IP, UDP_SERVER_PORT))

    #Update the graph (on main thread) - after 100 ms
    root.after(100, refresh_ui)

def makeSendFunc(msg):
    return lambda: sendUdpMessage(msg + "\n")

def listenUdpMessage_thread():
    #declare variable to allow effect on variable
    global data, readText   
    
    while True:
        data_raw, address = sock.recvfrom(4096)
        data = data_raw.decode()

        #print read data to screen
        print(data)
        display = (readText.get() + "\n" + data).strip()
        readText.set(display)

        #store into data -> update the data
        data = readTextToData(readText.get())
        print("DATA: \n", data)

########################
# DRAW GRAPH 
########################

def plot_data(data):
    # the figure that will contain the plot 
    fig = Figure(figsize = (5, 5), dpi = 100) 
    
    # Set title and axis text
    fig.suptitle('History Voltage Samples')
    fig.text(0.01, 0.5, 'Voltage', va='center', rotation='vertical')
    fig.text(0.5, 0.04, 'Samples', ha='center')
  
    # adding the subplot 
    plot = fig.add_subplot(111) 

    # Set vertical axis range
    plot.set_ylim(0, 1.9)

    # plotting the graph 
    plot.plot(data) 
  
    # creating the Tkinter canvas containing the Matplotlib figure 
    plotFrame = ttk.Frame(mainframe)
    plotFrame.grid(column=5, row=1, sticky=(N, W, E))
    canvas = FigureCanvasTkAgg(fig, master = plotFrame)
    canvas.draw() 
  
    # placing the canvas on the Tkinter window 
    canvas.get_tk_widget().pack() 


########################
# MAIN
########################

def main():
    openSocketPort()

    try:
        _thread.start_new_thread(listenUdpMessage_thread, ())
    except:
        print("Error: unable to start a thread")
        quit()
    
    #Setup Tk windown
    global root, mainframe, readText
    root = Tk()
    root.title("LIGHT DIP DETECTOR GUI")

    mainframe = ttk.Frame(root, padding="3 3 12 12")
    mainframe.grid(column=0, row=0, sticky=(N, W, E, S))
    root.columnconfigure(0, weight=1)
    root.rowconfigure(0, weight=1)

    #Text area to show received messages
    ttk.Label(mainframe, text="Read Text").grid(column=1, row=0, sticky=(W, E))
    readText = StringVar()
    readText.set("... <no data> ...")
        
    readTextFrame = ttk.Frame(mainframe, borderwidth=5, padding="3 3 12 12", width=700,height=1000, relief="ridge")
    readTextFrame.grid_propagate(False)
    readTextFrame.grid(column=1, row=1, rowspan=len(SEND_BUTTON_TEXTS), sticky=(W, E))
    readTextLabel = ttk.Label(readTextFrame, textvariable=readText, font="TkFixedFont",)
    readTextLabel.grid(column=0, row=0, sticky=(W, E))

    #Buttons to send commands
    ttk.Label(mainframe, text="Command").grid(column=0, row=0, sticky=(W, E))
    btnFrame = ttk.Frame(mainframe, padding="3 3 12 12")
    btnFrame.grid(column=0, row=1, sticky=(N, W, E))

    #Create buttons for each command
    for i, msg in enumerate(SEND_BUTTON_TEXTS):
        cmd = makeSendFunc(msg)
        ttk.Button(btnFrame, text=msg, command=cmd).grid(column=0, row=i, sticky=W)
    root.bind('<Return>', lambda event=None: sendUdpMessage("history\n"))
    
    # Show the plot initially.
    plot_data([0])

    # Configure grid widget
    for child in mainframe.winfo_children(): 
        child.grid_configure(padx=5, pady=5)

    root.mainloop()

if __name__ == "__main__":
    main()
