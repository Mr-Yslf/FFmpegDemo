#ifndef ENCODERTMP_H
#define ENCODERTMP_H

#include <QObject>
#include <QByteArray>
#include <QThread>
#include <QFile>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
}
#include <QContiguousCache>
class DecodeRtmp : public QObject
{
    Q_OBJECT
public:
    explicit DecodeRtmp(QObject *parent = 0);
    ~DecodeRtmp();

signals:
    void readyVideo(const QByteArray &data, int width, int height, int pixfmt);
    void readyAudio(const QByteArray &data);
public slots:
    void work();
private:
    int open_codec_context(AVMediaType type);
    void decode();
    int decode_packet(int & gotFrame, bool cached);
    int init();
    void release();

	struct _FFmpeg {
		AVFormatContext *fmtCtx;
		AVCodec *codec;
		AVCodecContext *codecCtx;
		AVStream	*stream;
		int streamIndex;

		_FFmpeg():fmtCtx(0),
			codec(0),
			codecCtx(0),
			stream(0),
			streamIndex(0)
		{
		}
	} FFmpeg;

	struct _Video : public _FFmpeg {
		uint8_t *dest_data[4];
		int dest_linesize[4];
		int dest_bufsize;
		enum AVPixelFormat pix_format;
		int frameWidth, frameHeight;
		QFile outFile;

	} video;
	struct _Audio :public _FFmpeg {

	}audio;
	AVFrame *frame;
	AVPacket packet;
	QByteArray audioData, videoData;
};
typedef struct _VideoData{
    QByteArray data;
    int width;
    int heigth;
    int pixfmt;
}VideoData;
class Work : public QObject
{
    Q_OBJECT
public:
    Work();
    ~Work();
    VideoData getData(int &got);
signals:
    void readyVideo(const QByteArray &data, int width, int height, int pixfmt);
    void readyAudio(const QByteArray &data);
    void work();
public slots:
    void processVideo(const QByteArray &data, int width, int height, int pixfmt);
private:
    QThread thread;
    DecodeRtmp *himma;

    QContiguousCache <VideoData> cache;
};
#endif // ENCODERTMP_H
