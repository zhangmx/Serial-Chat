#ifndef SERIAL_PORT_REMARK_DIALOG_H
#define SERIAL_PORT_REMARK_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

/**
 * @brief Dialog for setting a remark/alias for a serial port
 */
class SerialPortRemarkDialog : public QDialog {
    Q_OBJECT

public:
    explicit SerialPortRemarkDialog(const QString& portName, QWidget* parent = nullptr);
    SerialPortRemarkDialog(const QString& portName, const QString& currentRemark, QWidget* parent = nullptr);
    ~SerialPortRemarkDialog() override;
    
    // Get the entered remark
    QString remark() const;
    
    // Set initial remark
    void setRemark(const QString& remark);

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onClearClicked();

private:
    QString m_portName;
    
    // UI Components
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_portLabel;
    QLineEdit* m_remarkEdit;
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_clearButton;
    QPushButton* m_cancelButton;
    QPushButton* m_okButton;
    
    void setupUi();
};

#endif // SERIAL_PORT_REMARK_DIALOG_H
