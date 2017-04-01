from PyQt4 import QtCore, QtGui

# Collapsible GroupBox -- because QGroupBox just doesn't have that feature.
class CollapsibleGroupBox(QtGui.QGroupBox):
    def __init__(self, title, parent=None):
        super(CollapsibleGroupBox, self).__init__(title, parent)
        self.setCheckable(True)
        self.setChecked(True)
        self.connect(self, QtCore.SIGNAL('clicked(bool)'),
                lambda chk: self.expand() if chk else self.collapse())
        # Put a widget inside the groupbox. This widget will be
        # made visible when expanding/collapsing.
        self.setLayout(QtGui.QFormLayout())
        self.containerWidget = QtGui.QWidget()
        self.containerWidget.setLayout(QtGui.QVBoxLayout())
        self.containerWidget.layout().setMargin(0)
        self.layout().addWidget(self.containerWidget)

    def innerLayout(self):
        return self.containerWidget.layout()

    def expand(self):
        self.containerWidget.setVisible(True)
        self.setChecked(True)

    def collapse(self):
        self.containerWidget.setVisible(False)
        self.setChecked(False)

