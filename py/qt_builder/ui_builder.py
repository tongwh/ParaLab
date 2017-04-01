# TODO:
# Write doc about how to add new widget
# - It is important to make sure that the lambda don't get garbage collected.
# - IF a checkbox has a hotkey, then it shouldn't rely on lambda having a
#   parameter (hotkeys are computed to be global) and should instead rely on
#   a name.
# - If action and postAction only happen if preAction returns True
# Add
# - Widget types:
#       - File selector (with a browser button)
#       - Spinners?
#       - Shortcut pseudo-widget?
# - Add support for different layouts?
# - Turn on shortcut underlining on Windows
# XXX - (1) auto-switch to group box
#       (2) refresh button for group box
# - There should be a sub-form type, which is a collapsible group with an
#   "OK" and an optional "RESET" button.


from PyQt4 import QtCore, QtGui
from custom_widgets import *
import inspect   # for inspect.isfunction()

# Widget identifiers #  mandatory modifiers   # optional modifiers (any object=a, boolean=b, int=i, str=s)
UI_TABS      = 1     #                        # scrollbars (b)
UI_WIDGET    = 2     #                        #
UI_GROUP_BOX = 3     # label (s)              # name (a), checkable (b)  OR  collapsible (b), start_collapsed (b)
UI_BUTTON    = 4     # label (s)              # name (a), flat (b), disabled (b)
UI_CHECK_BOX = 5     # label (s)              # name (a), checked (b), shortcut (s)
UI_COMBO_BOX = 6     #                        # name (a), label (s), selected (i), int_returned (b, lambda received int instead of str index)
UI_TEXT_BOX  = 7     #                        # name (a), label (s), shortcut (s), text (s), validator (2-tuple (min, max) for int and 3-tuple (min, max, #digits) for double)
UI_LABEL     = 8     # label (s)              # name (a)


class Environment(object):
    def __init__(self):
        self.env = {}
    def get(self, name):
        assert name in self.env, 'Cannot get widget data: no widget with name ' + repr(name)
        return self.env.get(name).get()
    def set(self, name, val):
        assert name in self.env, 'Cannot set widget data: no widget with name ' + repr(name)
        self.env[name].set(val)
        return self.env.get(name).get()
    def widget(self, name):
        assert name in self.env, 'Cannot get widget: no widget with name ' + repr(name)
        return self.env.get(name).getWidget()
    def __getitem__(self, name):
        return self.get(name)
    def __setitem__(self, name, val):
        return self.set(name, val)
    def add(self, name, widget):
        if not name: return # only add widgets with names
        assert name not in self.env, 'Cannot add widget with name ' + repr(name) + ' as one already exists'
        self.env[name] = _WidgetVar(widget)
    def getInt(self, name):
        try:
            return int(self.get(name))
        except (ValueError, TypeError):
            print('ERROR: \'%s\' is a not a valid integer value' % self.get(name))
            return None
    def getFloat(self, name):
        try:
            return float(self.get(name))
        except (ValueError, TypeError):
            print('ERROR: \'%s\' is a not a valid floating point value' % self.get(name))
            return None


# Easy way to access/modify values within widgets
# This is only needed for widgets (e.g. QCheckBox, QComboBox, or QLineEdit)
# that allow access to a value.
class _WidgetVar(object):
    def __init__(self, _widget):
        self.widget = _widget
        if type(_widget) not in ( QtGui.QCheckBox, QtGui.QComboBox,
                QtGui.QLineEdit, QtGui.QLabel,
                QtGui.QGroupBox, CollapsibleGroupBox ):
            assert False, '\'name\' parameter not supported for ' + repr(type(_widget)) + ' widget type'
    def get(self):
        t = type(self.widget)
        if t == QtGui.QCheckBox:
            return self.widget.isChecked()
        elif t == QtGui.QComboBox: # getter gives [int, str], setter uses an int
            return [self.widget.currentIndex(), self.widget.currentText()]
        elif t == QtGui.QLineEdit or t == QtGui.QLabel:
            return self.widget.text()
        elif t == QtGui.QGroupBox or t == CollapsibleGroupBox:
            return self.widget.title()
        else:
            assert False, '_WidgetVar: get not supported for ' + repr(type(self.widget)) + ' widget type'
    def set(self, val):
        t = type(self.widget)
        if t == QtGui.QCheckBox:
            self.widget.setChecked(bool(val))
            return self.widget.isChecked()
        elif t == QtGui.QComboBox: # getter gives [int, str], setter uses an int
            self.widget.setCurrentIndex(val)
            return self.widget.currentIndex()
        elif t == QtGui.QLineEdit or t == QtGui.QLabel:
            self.widget.setText(str(val))
            return self.widget.text()
        elif t == QtGui.QGroupBox or t == CollapsibleGroupBox:
            self.widget.setTitle(str(val))
            return self.widget.title()
        else:
            assert False, '_WidgetVar: set not supported for ' + repr(type(self.widget)) + ' widget type'
    def getWidget(self):
        return self.widget


def _trans(str):
    return QtCore.QCoreApplication.translate('QtGui.QMainWindow', str)

# Adds a list of widgets to a given layout
def _addWidgetsToLayout(lyt, widgetList, env, widgetForShortcuts, preAction, postAction, postCreationActions):
    for w in widgetList:
        lyt.addWidget(_recursiveCreateWidgets(w, env, widgetForShortcuts, preAction, postAction, postCreationActions))

# Menu list format: item label, lambda callback
def _addPopupMenu(parent, menuList):
    menu = QtGui.QMenu(parent)
    for item in menuList:
        menu.addAction(_trans(item[0]), item[1])
    parent.setMenu(menu)

# If label is None, the widget is returned
# Otherwise, a new widget is returned, containing the label and the widget
# that was provided.
def _addLabelToWidget(label, widget):
    if label == None:
        return widget

    lbl = QtGui.QLabel(_trans(label))
    lbl.setBuddy(widget)
    lblContainer = QtGui.QWidget()
    lblContainer.setLayout(QtGui.QHBoxLayout())
    lblContainer.layout().addWidget(lbl)
    lblContainer.layout().addWidget(widget)
    lblContainer.layout().setMargin(0)
    return lblContainer

def _wrapAction(name, action, preAction, postAction):
    def _wrappedAction(name, action, preAction, postAction, *args):
        if preAction and not preAction(name):
            return
        val = action(*args)
        postAction and postAction(name)
        return val

    if not inspect.isfunction(action) or (not preAction and not postAction):
        return action
    argcount = action.func_code.co_argcount
    if argcount == 0:
        return lambda: _wrappedAction(name, action, preAction, postAction)
    elif argcount == 1:
        return lambda a: _wrappedAction(name, action, preAction, postAction, a)
    elif argcount == 2:
        return lambda a, b: _wrappedAction(name, action, preAction, postAction, a, b)
    elif argcount == 3:
        return lambda a, b, c: _wrappedAction(name, action, preAction, postAction, a, b, c)
    else:
        assert False, '_wrapAction: Don\'t yet support ' + str(argcount) + ' arguments'

# Recursively create widgets
# - connector is some persistent QObject (since it needs to be alive for the
#   signal/slot connections to persist)
def recursiveCreateWidgets(guiNode, widgetForShortcuts = None, env=None, preAction=None, postAction=None):
    # dictionary of label names-to-widget mappings
    if not env:
        env = Environment()
    postCreationActions = [] # these actions are intended to be run after show() has been called once
    widget = _recursiveCreateWidgets(guiNode, env, widgetForShortcuts, preAction, postAction, postCreationActions)
    widget.show()
    for action in postCreationActions:
        action()
    return (widget, env)

def _recursiveCreateWidgets(guiNode, env, widgetForShortcuts, preAction, postAction, postCreationActions):
    assert isinstance(guiNode, list), 'Array expected. Received ' + repr(guiNode)

    # Extract common parameters
    #if guiNode[0] in (UI_TABS, UI_WIDGET):
    #    widgetType = guiNode[0]
    #    widgetOptions = {}
    #else:
    widgetType = guiNode[0]
    widgetOptions = guiNode[1]
    assert isinstance(widgetOptions, dict), 'Expected hash map of options. Received ' + repr(widgetOptions)

    widgetLabel = widgetOptions.get('label')          # can be None if label optional
    widgetShortcut = widgetOptions.get('shortcut')    # can be None if shortcut optional
    if widgetLabel and widgetShortcut:
        widgetLabel = "%s {%s}" % (widgetLabel, widgetShortcut)
    widgetName = widgetOptions.get('name')            # can be None if name optional
    widgetAction = _wrapAction(widgetName, guiNode[2], preAction, postAction) if len(guiNode) > 2 else None
    widget = None

    # Checkbox
    # Format: type, options, action
    if widgetType == UI_CHECK_BOX:
        widget = QtGui.QCheckBox(_trans(widgetLabel))
        env.add(widgetName, widget)  # name
        widget.setChecked(bool(widgetOptions.get('checked')))
        if widgetAction:             # action
            QtCore.QObject.connect(widget, QtCore.SIGNAL('clicked(bool)'), widgetAction)
            if widgetShortcut: # shortcut for action
                if widgetForShortcuts:
                    # This doesn't work when the widget is not "visible"
                    # (e.g. in a collapsed GroupBox)
                    #QtGui.QShortcut(_trans(widgetShortcut), widgetForShortcuts, lambda: widget.animateClick())

                    # If the shortcut is global, then the checkbox needs to be
                    # queried and updated since the shortcut is not directly related
                    # to the checkbox.
                    checkboxAction = lambda: [
                            widget.toggle(),
                            widgetAction(widget.isChecked()) if widgetAction.func_code.co_argcount == 1 else widgetAction()
                            ]
                    QtGui.QShortcut(_trans(widgetShortcut), widgetForShortcuts, checkboxAction)
                else:
                    # This doesn't work when the button is not in view or not "visible"
                    widget.setShortcut(_trans(widgetShortcut))

    # Button
    # Format: type, options, action    OR    type, options, menu label list
    elif widgetType == UI_BUTTON:
        widget = QtGui.QPushButton(_trans(widgetLabel))
        if widgetOptions.get('flat'): # flat button
            widget.setFlat(True)
        if widgetOptions.get('disabled'): # disabled
            widget.setEnabled(False)
        if isinstance(widgetAction, list): # popup button
            _addPopupMenu(widget, guiNode[2:])
        elif widgetAction: # action
            QtCore.QObject.connect(widget, QtCore.SIGNAL('clicked(bool)'), widgetAction)
            if widgetShortcut: # shortcut for action
                if widgetForShortcuts:
                    # This doesn't work when the widget is not "visible"
                    # (e.g. in a collapsed GroupBox)
                    #QtGui.QShortcut(_trans(widgetShortcut), widgetForShortcuts, lambda: widget.animateClick())

                    # Widget action must take 0 arguments for the hotkey to work
                    QtGui.QShortcut(_trans(widgetShortcut), widgetForShortcuts, widgetAction)
                else:
                    # This doesn't work when the button is not in view or not "visible"
                    widget.setShortcut(_trans(widgetShortcut))

    # Combobox
    # Format: type, options, action, string list of options
    elif widgetType == UI_COMBO_BOX:
        optionsList = guiNode[3]
        assert isinstance(optionsList, list), 'Expected list of options for combo box. Received ' + repr(list)
        comboWidget = QtGui.QComboBox()
        comboWidget.addItems(optionsList)
        idx = widgetOptions.get('selected')
        comboWidget.setCurrentIndex(-1 if idx == None else idx)
        if widgetAction:
            if widgetOptions.get('int_returned'):
                QtCore.QObject.connect(comboWidget, QtCore.SIGNAL('currentIndexChanged(int)'), widgetAction)
            else:
                QtCore.QObject.connect(comboWidget, QtCore.SIGNAL('currentIndexChanged(const QString &)'), widgetAction)
        env.add(widgetName, comboWidget)
        widget = _addLabelToWidget(widgetLabel, comboWidget)

    # Input textbox
    # Format: type, options, action
    elif widgetType == UI_TEXT_BOX:
        textWidget = QtGui.QLineEdit()
        if widgetAction:
            QtCore.QObject.connect(textWidget, QtCore.SIGNAL('editingFinished()'), widgetAction)
        text = str(widgetOptions.get('text'))
        textWidget.setText(_trans('' if text == None else text))
        vOption = widgetOptions.get('validator')
        if vOption:
            assert isinstance(vOption, tuple), 'Validator option on text box must be a (min, max) tuple'
            if len(vOption) == 3:
                validator = QtGui.QDoubleValidator(vOption[0], vOption[1], vOption[2], textWidget)
            else:
                validator = QtGui.QIntValidator(vOption[0], vOption[1], textWidget)
            textWidget.setValidator(validator)
        env.add(widgetName, textWidget)
        widget = _addLabelToWidget(widgetLabel, textWidget)

    # Label (read-only textbox)
    # Format: type, options
    elif widgetType == UI_LABEL:
        widget = QtGui.QLabel()
        widget.setText(_trans(str(widgetLabel)))
        env.add(widgetName, widget)

    # Container widgets
    # Format: type, options, list of widgets
    elif widgetType == UI_GROUP_BOX:
        widgetList  = guiNode[2:]

        # Collapsible version
        if widgetOptions.get('collapsible'):
            widget = CollapsibleGroupBox(_trans(widgetLabel))
            _addWidgetsToLayout(widget.innerLayout(), widgetList, env, widgetForShortcuts, preAction, postAction, postCreationActions)
            # Collapse the widget only after its children have been added
            if widgetOptions.get('collapsible') and widgetOptions.get('start_collapsed'):
                # need to show the widget once before collapsing for better sizing
                postCreationActions.append(lambda: widget.collapse())

        # Regular version
        else:
            widget = QtGui.QGroupBox(_trans(widgetLabel))
            if widgetOptions.get('checkable'):
                widget.setCheckable(True)
            widget.setLayout(QtGui.QFormLayout())
            _addWidgetsToLayout(widget.layout(), widgetList, env, widgetForShortcuts, preAction, postAction, postCreationActions)

        env.add(widgetName, widget)

    # Tab widget
    # Format: type, list of tabs
    # Format for each tab: tab label, list of widgets
    elif widgetType == UI_TABS:
        widget = QtGui.QTabWidget()
        scrollbars = True;
        if 'scrollbars' in widgetOptions:
            scrollbars = widgetOptions.get('scrollbars')
        for tab in guiNode[2:]: # create each tab
            tabLabel   = tab[0]
            widgetList = tab[1:]
            tabWidget  = QtGui.QWidget()
            tabWidget.setLayout(QtGui.QFormLayout())
            _addWidgetsToLayout(tabWidget.layout(), widgetList, env, widgetForShortcuts, preAction, postAction, postCreationActions)
            if scrollbars:
                # allow scrolling
                scrollArea = QtGui.QScrollArea()
                scrollArea.setWidget(tabWidget)
                scrollArea.setWidgetResizable(True)
                tabWidget.show()
                widget.addTab(scrollArea, _trans(tabLabel))
            else:
                widget.addTab(tabWidget, _trans(tabLabel))

    # Tab widget
    # Format: type, list of elements
    # Format for each tab: tab label, list of widgets
    elif widgetType == UI_WIDGET:
        container = QtGui.QWidget()
        widgetList = guiNode[1:]
        container.setLayout(QtGui.QFormLayout())
        _addWidgetsToLayout(container.layout(), widgetList, env, widgetForShortcuts, preAction, postAction, postCreationActions)
        # allow scrolling
        widget = QtGui.QScrollArea()
        widget.setWidget(container)
        widget.setWidgetResizable(True)
        container.show()

    # Unsupported widget
    else:
        assert False, 'Unknown widget type ' + repr(widgetType)

    return widget

# Test
if __name__ == '__main__':
    def _myPrint(str):
        print(str)

    counter = 0
    def _doWorkAndUpdateStatusBar():
        global window, counter
        counter += 1
        window.statusBar().showMessage('Action counter: ' + str(counter))

    test = [ UI_TABS, {},
        [ 'Tab &1',
            [ UI_CHECK_BOX, {'label': 'No action'}, None ],
            [ UI_CHECK_BOX, {'label': 'No action2'} ],
            [ UI_TABS, {},
                [ 'Tab &C',
                    [ UI_CHECK_BOX, {'label': '&Flag 1', 'shortcut': 'Ctrl+f'}, lambda flag: _myPrint('&Flag 1 ' + str(flag)) ],
                    [ UI_CHECK_BOX, {'label': 'F&lag 2'}, lambda flag: _myPrint('F&lag 2 ' + str(flag)) ],
                ],
                [ 'Tab &D',
                    [ UI_COMBO_BOX, {'selected': 2, 'int_returned': True},
                        lambda i: _myPrint('Option ' + str(i) + ' chosen'),
                        [ 'a option', 'b option', 'c option', 'd option' ]],
                    [ UI_COMBO_BOX, {'label': 'Options', 'selected': 2 },
                        lambda s: _myPrint('Option ' + str(s) + ' chosen'),
                        [ 'a option', 'b option', 'c option', 'd option' ]],
                ],
            ],
            [ UI_GROUP_BOX, {'label': 'Collapsible group', 'collapsible': True},
                [ UI_CHECK_BOX, {'label': 'Flag &a', 'shortcut': 'a', 'checked': True}, lambda flag: _myPrint('Flag &a ' + str(flag)) ],
                [ UI_CHECK_BOX, {'label': 'Flag &b', 'shortcut': 'b'                 }, lambda flag: _myPrint('Flag &b ' + str(flag)) ]],
            [ UI_GROUP_BOX, {'label': 'Collapsed collapsible group', 'collapsible': True, 'start_collapsed': True}, 
                [ UI_CHECK_BOX, {'label': 'Flag &c', 'shortcut': 'c', 'checked': True}, lambda flag: _myPrint('Flag &c ' + str(flag)) ],
                [ UI_CHECK_BOX, {'label': 'Flag &d', 'shortcut': 'd'                 }, lambda flag: _myPrint('Flag &d ' + str(flag)) ]],
            [ UI_BUTTON, {'label': 'Push button'},
                [ 'Menu Item 1', lambda: _myPrint('Menu Item 1') ],
                [ 'Menu Item 2', lambda: _myPrint('Menu Item 2') ]],
            [ UI_BUTTON, {'label': 'Button!'}, lambda flag: _myPrint('Button!' + str(flag)) ],
            [ UI_BUTTON, {'label': 'Inspect (Ctrl+i)', 'shortcut': 'Ctrl+i'},
                lambda: _myPrint('Got [%s] [%s] [%s]' % ( repr(env['check']), repr(env.getFloat('num')), repr(env['combo']) )) ],
        ],
        [ 'Tab &2',
            [ UI_GROUP_BOX, {'label': 'Checkable group', 'checkable': True},
                [ UI_BUTTON   , {'label': 'Button 1'  }, lambda flag: _myPrint('Button 1 '   + str(flag)) ],
                [ UI_CHECK_BOX, {'label': 'Checkbox 1'}, lambda flag: _myPrint('Checkbox 1 ' + str(flag)) ]],
            [ UI_TEXT_BOX, {}, lambda: _myPrint('Text box!') ],
            [ UI_TEXT_BOX, {'label': 'Int box (1..10)',          'text': '30' , 'validator': (1  , 10     )}, lambda: _myPrint('Int box') ],
            [ UI_TEXT_BOX, {'label': 'Double box (1.00..10.00)', 'name': 'num', 'validator': (1.0, 10.0, 2)}, lambda: _myPrint('Double box') ],
            [ UI_CHECK_BOX, {'label': 'Yes or No', 'name': 'check'} ],
            [ UI_COMBO_BOX, {'label': 'Choose!'  , 'name': 'combo', 'selected': -1}, None, ['Option 1', 'Option 2', 'Option 3'] ],
            [ UI_BUTTON, {'label': 'Set &widgets! (Ctrl+s)', 'shortcut': 'Ctrl+s', 'flat': True},
                lambda: _myPrint([ env.set('check', 5), env.set('num', 10), env.set('combo', 2), _doWorkAndUpdateStatusBar() ]) ],
            [ UI_LABEL, {'label': '*** Just a label ***', 'name': 'inc.label'} ],
            [ UI_BUTTON, {'label': 'Increment label (Ctrl+L)', 'shortcut': 'Ctrl+l'},
                lambda: _myPrint( env.set('inc.label', env.getInt('inc.label')+1) ) ],
        ],
    ]

    import sys
    app = QtGui.QApplication(sys.argv)
    window = QtGui.QMainWindow()
    (sidebar, env) = recursiveCreateWidgets(test, window)
    window.setCentralWidget(sidebar)
    window.setStatusBar(QtGui.QStatusBar())
    window.statusBar().showMessage('This is the status bar!')
    window.show()

    sys.exit(app.exec_())

