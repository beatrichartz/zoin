#include "qrcodedialog.h"
#include "ui_qrcodedialog.h"

#include "bitcoinunits.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "optionsmodel.h"

#include <QPixmap>
#if QT_VERSION < 0x050000
#include <QUrl>
#endif

#include <qrencode.h>

QRCodeDialog::QRCodeDialog(const QString &addr,  const QString &label, bool enableReq, QWidget *parent,  const QString &priv, bool paperWallet) :
    QDialog(parent),
    ui(new Ui::QRCodeDialog),
    model(0),
    priv(priv),
    address(addr)
{
    ui->setupUi(this);

    //setWindowTitle(QString("%1").arg(address));

    setWindowTitle(QString("Zoin Paper Wallet"));
    //ui->chkReqPayment->setVisible(enableReq);
    //ui->lblAmount->setVisible(enableReq);
    //ui->lnReqAmount->setVisible(enableReq);

    //ui->lnLabel->setText(label);

    ui->btnSaveAs->setEnabled(false);

    if(paperWallet){
        ui->title->setText(label + " Paper Wallet");
        genCodePriv();
    }
    genCodePub();
}

QRCodeDialog::~QRCodeDialog()
{
    delete ui;
}

void QRCodeDialog::setModel(OptionsModel *model)
{
    this->model = model;

    if (model)
        connect(model, SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

    // update the display unit, to not use the default ("BTC")
    updateDisplayUnit();
}

void QRCodeDialog::genCodePub()
{
    QString uri = getURIPub();

    if (uri != "")
    {
        ui->lblQRCode_pub->setText("");

        QRcode *code = QRcode_encodeString(uri.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
        if (!code)
        {
            ui->lblQRCode_pub->setText(tr("Error encoding URI into QR Code."));
            return;
        }
        myImage = QImage(code->width + 8, code->width + 8, QImage::Format_RGB32);
        myImage.fill(0xffffff);
        unsigned char *p = code->data;
        for (int y = 0; y < code->width; y++)
        {
            for (int x = 0; x < code->width; x++)
            {
                myImage.setPixel(x + 4, y + 4, ((*p & 1) ? 0x0 : 0xffffff));
                p++;
            }
        }
        QRcode_free(code);

        ui->lblQRCode_pub->setPixmap(QPixmap::fromImage(myImage).scaled(300, 300));

        ui->outUri_pub->setText(uri);
    }
}

QString QRCodeDialog::getURIPub()
{
    QString ret = QString("zoin:%1").arg(address);
    int paramCount = 0;

    ui->outUri_pub->clear();


    // limit URI length to prevent a DoS against the QR-Code dialog
    if (ret.length() > MAX_URI_LENGTH)
    {
        ui->btnSaveAs->setEnabled(false);
        ui->lblQRCode_pub->setText(tr("Resulting URI too long, try to reduce the text for label / message."));
        return QString("");
    }

    ui->btnSaveAs->setEnabled(true);
    return ret;
}


void QRCodeDialog::genCodePriv()
{
    QString uri = getURIPriv();

    if (uri != "")
    {
        ui->lblQRCode_priv->setText("");

        QRcode *code = QRcode_encodeString(uri.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
        if (!code)
        {
            ui->lblQRCode_priv->setText(tr("Error encoding URI into QR Code."));
            return;
        }
        myImage = QImage(code->width + 8, code->width + 8, QImage::Format_RGB32);
        myImage.fill(0xffffff);
        unsigned char *p = code->data;
        for (int y = 0; y < code->width; y++)
        {
            for (int x = 0; x < code->width; x++)
            {
                myImage.setPixel(x + 4, y + 4, ((*p & 1) ? 0x0 : 0xffffff));
                p++;
            }
        }
        QRcode_free(code);

        ui->lblQRCode_priv->setPixmap(QPixmap::fromImage(myImage).scaled(300, 300));

        ui->outUri_priv->setText(uri);
    }
}

QString QRCodeDialog::getURIPriv()
{
    QString ret = QString("zoin:%1").arg(priv);
    int paramCount = 0;

    ui->outUri_priv->clear();


    // limit URI length to prevent a DoS against the QR-Code dialog
    if (ret.length() > MAX_URI_LENGTH)
    {
        ui->btnSaveAs->setEnabled(false);
        ui->lblQRCode_priv->setText(tr("Resulting URI too long, try to reduce the text for label / message."));
        return QString("");
    }

    ui->btnSaveAs->setEnabled(true);
    return ret;
}

void QRCodeDialog::on_lnReqAmount_textChanged()
{
    genCodePub();
}

void QRCodeDialog::on_lnLabel_textChanged()
{
    genCodePub();
}

void QRCodeDialog::on_lnMessage_textChanged()
{
    genCodePub();
}

void QRCodeDialog::on_btnSaveAs_clicked()
{

    QString fn = GUIUtil::getSaveFileName(this, tr("Save QR Code"), QString(), tr("PNG Images (*.png)"), NULL);
    if (!fn.isEmpty())
        //myImage.scaled(QR_IMAGE_SIZE, QR_IMAGE_SIZE).save(fn);
        this->grab().save(fn);
}

void QRCodeDialog::on_chkReqPayment_toggled(bool fChecked)
{
    genCodePub();
}

void QRCodeDialog::updateDisplayUnit()
{

}
