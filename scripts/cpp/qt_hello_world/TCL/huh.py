#!/usr/bin/python3

import tkinter as tk
from tkinter import messagebox

class App(tk.Tk):
    def __init__(self):
        super(App, self).__init__()
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.trayMenu = None

    def on_closing(self):
        if not self.trayMenu: # when system tray is not exists.
            selection = messagebox.askyesnocancel("Tips", "Quit directly?\nYes : Quit.\nNo:Minimize to system tray.")  # "Yes" will return True, "Cancel" will return None, "No" will return False.
            if selection: # when select yes, quit the app directly.
                self.destroy()
            elif selection == False: # Minimize to system tray.
                # make a system tray
                self.withdraw()
                # use bulitin tk.Menu

                # The work about "Winico"
                self.tk.call('package', 'require', 'Winico') # use the tcl "winico", make sure the folder of "winico" is in the same path.
                icon = self.tk.call('winico', 'createfrom', '2.ico') # this is the icon on the system tray.
                self.tk.call('winico', 'taskbar', 'add', icon, # set the icon
                            '-callback', (self.register(self.menu_func), '%m', '%x', '%y'), # refer to winico documentation.
                            '-pos', 0,
                            '-text', u'jizhihaoSAMA\u2019s Tool') # the hover text of the system tray.

                # About menu
                self.trayMenu = tk.Menu(self, tearoff=False)
                self.trayMenu.add_command(label="Show my app", command=self.deiconify)

                # You could also add a cascade menu
                cascadeMenu = tk.Menu(self, tearoff=False)
                cascadeMenu.add_command(label="Casacde one", command=lambda :print("You could define it by yourself"))
                cascadeMenu.add_command(label="Cascade two")
                self.trayMenu.add_cascade(label="Other", menu=cascadeMenu)

                self.trayMenu.add_separator() # you could add a separator

                self.trayMenu.add_command(label="Quit", command=self.destroy)

                # you could also add_command or add_checkbutton for what you want
            else: # This is cancel operation
                pass
        else:
            self.withdraw() # when system tray exists, hide the window directly.

    def menu_func(self, event, x, y):
        if event == 'WM_RBUTTONDOWN': # Mouse event, Right click on the tray.Mostly we will show it.
            self.trayMenu.tk_popup(x, y) # pop it up on this postion
        if event == 'WM_LBUTTONDOWN': # Mouse event, Left click on the tray,Mostly we will show the menu.
            self.deiconify() # show it.
        # All the Mouse event:

        # WM_MOUSEMOVE
        # WM_LBUTTONDOWN
        # WM_LBUTTONUP
        # WM_LBUTTONDBLCLK
        # WM_RBUTTONDOWN
        # WM_RBUTTONUP
        # WM_RBUTTONDBLCLK
        # WM_MBUTTONDOWN
        # WM_MBUTTONUP
        # WM_MBUTTONDBLCLK

app = App()
app.mainloop()
