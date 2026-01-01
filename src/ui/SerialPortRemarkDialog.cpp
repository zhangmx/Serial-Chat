#include "SerialPortRemarkDialog.h"

SerialPortRemarkDialog::SerialPortRemarkDialog(const QString& portName, QWidget* parent)
    : QDialog(parent)
    , m_portName(portName)
{
    setupUi();
}

SerialPortRemarkDialog::SerialPortRemarkDialog(const QString& portName, const QString& currentRemark, QWidget* parent)
    : QDialog(parent)
    , m_portName(portName)
{
    setupUi();
    setRemark(currentRemark);
}

SerialPortRemarkDialog::~SerialPortRemarkDialog()
{
}

QString SerialPortRemarkDialog::remark() const
{
    return m_remarkEdit->text().trimmed();
}

void SerialPortRemarkDialog::setRemark(const QString& remark)
{
    m_remarkEdit->setText(remark);
}

void SerialPortRemarkDialog::onOkClicked()
{
    accept();
}

void SerialPortRemarkDialog::onCancelClicked()
{
    reject();
}

void SerialPortRemarkDialog::onClearClicked()
{
    m_remarkEdit->clear();
}

void SerialPortRemarkDialog::setupUi()
{
    setWindowTitle(tr("Set Port Remark"));
    setMinimumWidth(300);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    m_titleLabel = new QLabel(tr("Set a custom name for this port:"), this);
    
    m_portLabel = new QLabel(m_portName, this);
    m_portLabel->setStyleSheet("font-weight: bold; color: #1976D2;");
    
    m_remarkEdit = new QLineEdit(this);
    m_remarkEdit->setPlaceholderText(tr("Enter remark (e.g., Arduino, GPS Module)"));
    m_remarkEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #E0E0E0; border-radius: 5px; }");
    
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(10);
    
    m_clearButton = new QPushButton(tr("Clear"), this);
    m_clearButton->setStyleSheet("QPushButton { padding: 8px 15px; }");
    connect(m_clearButton, &QPushButton::clicked, this, &SerialPortRemarkDialog::onClearClicked);
    
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_cancelButton->setStyleSheet("QPushButton { padding: 8px 15px; }");
    connect(m_cancelButton, &QPushButton::clicked, this, &SerialPortRemarkDialog::onCancelClicked);
    
    m_okButton = new QPushButton(tr("OK"), this);
    m_okButton->setDefault(true);
    m_okButton->setStyleSheet("QPushButton { padding: 8px 25px; background-color: #07C160; color: white; border: none; border-radius: 5px; } QPushButton:hover { background-color: #06AD56; }");
    connect(m_okButton, &QPushButton::clicked, this, &SerialPortRemarkDialog::onOkClicked);
    
    m_buttonLayout->addWidget(m_clearButton);
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addWidget(m_okButton);
    
    m_mainLayout->addWidget(m_titleLabel);
    m_mainLayout->addWidget(m_portLabel);
    m_mainLayout->addWidget(m_remarkEdit);
    m_mainLayout->addStretch();
    m_mainLayout->addLayout(m_buttonLayout);
}
