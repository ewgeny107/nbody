#include <QString>
#include <QFile>
#include <QtTest>

#include "nbody_solvers.h"
#include "nbody_engines.h"
#include "nbody_data_stream.h"
#include "nbody_data_stream_reader.h"

class test_nbody_stream : public QObject
{
	Q_OBJECT

	nbody_data			m_data;
	nbody_engine*		m_e;
	nbody_solver*		m_s;
	nbody_data_stream*	m_stream;
public:
	test_nbody_stream();
	~test_nbody_stream();
private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void run();
	void negative_branch();
};

test_nbody_stream::test_nbody_stream() :
	m_e(new nbody_engine_simple()),
	m_s(new nbody_solver_euler()),
	m_stream(new nbody_data_stream())
{

}

test_nbody_stream::~test_nbody_stream()
{
	delete m_s;
	delete m_e;
	delete m_stream;
}

void test_nbody_stream::initTestCase()
{
	nbcoord_t			box_size = 100;

	qDebug() << "Solver =" << m_s->type_name() << "engine" << m_e->type_name();
	m_data.make_universe(64, box_size, box_size, box_size);

	m_e->init(&m_data);
	m_s->set_time_step(1e-3, 3e-2);
	m_s->set_engine(m_e);
}

void test_nbody_stream::cleanupTestCase()
{
}

void test_nbody_stream::run()
{
	m_s->set_time_step(0.01, 0.1);
	//open stream with 2 frames per data file limit
	QVERIFY(0 == m_stream->open("/tmp/stream-test/new", 14000));
	QVERIFY(0 == m_s->run(&m_data, m_stream, 0.31, 0.1, 0.1));

	m_stream->close();

	QVERIFY(QFile::exists("/tmp/stream-test/new.idx"));
	QVERIFY(QFile::exists("/tmp/stream-test/new0.dat"));
	QVERIFY(QFile::exists("/tmp/stream-test/new1.dat"));
	QVERIFY(QFile::exists("/tmp/stream-test/new2.dat"));

	QFile	idx("/tmp/stream-test/new.idx");

	QVERIFY(idx.open(QFile::ReadOnly));

	QVERIFY(QString(idx.readAll()).split(QChar('\n'), QString::SkipEmptyParts).size() == 5);

	{
		nbody_data_stream_reader	reader;
		QVERIFY(0 == reader.load("/tmp/stream-test/new"));
		QVERIFY(5 == reader.get_frame_count());
		QVERIFY(4 == reader.get_steps_count());
		QVERIFY(0.4 == reader.get_max_time());

		QByteArray	yexpected(static_cast<int>(m_e->get_y()->size()), static_cast<char>(0xCC));
		QByteArray	ycurrent(static_cast<int>(m_e->get_y()->size()), static_cast<char>(0xCC));

		m_e->read_buffer(yexpected.data(), m_e->get_y());

		QVERIFY(0 == reader.seek(0));
		QVERIFY(0 != reader.seek(77));

		QVERIFY(0 == reader.seek(0));
		QVERIFY(0 == reader.read(m_e));
		m_e->read_buffer(ycurrent.data(), m_e->get_y());
		QVERIFY(ycurrent != yexpected);

		QVERIFY(0 == reader.seek(reader.get_frame_count() - 1));
		QVERIFY(0 == reader.read(m_e));
		m_e->read_buffer(ycurrent.data(), m_e->get_y());
		QVERIFY(ycurrent == yexpected);

		QVERIFY(0 == reader.seek(0));

		QVERIFY(0 == reader.read(m_e));
		m_e->read_buffer(ycurrent.data(), m_e->get_y());
		QVERIFY(ycurrent != yexpected);
		QVERIFY(0 == m_e->get_time());
		QVERIFY(0 == m_e->get_step());

		QVERIFY(0 == reader.read(m_e));
		m_e->read_buffer(ycurrent.data(), m_e->get_y());
		QVERIFY(ycurrent != yexpected);

		QVERIFY(0 == reader.read(m_e));
		m_e->read_buffer(ycurrent.data(), m_e->get_y());
		QVERIFY(ycurrent != yexpected);

		QVERIFY(0 == reader.read(m_e));
		m_e->read_buffer(ycurrent.data(), m_e->get_y());
		QVERIFY(ycurrent != yexpected);
		QVERIFY(0.3 == m_e->get_time());
		QVERIFY(3 == m_e->get_step());

		QVERIFY(0 == reader.read(m_e));
		m_e->read_buffer(ycurrent.data(), m_e->get_y());
		QVERIFY(ycurrent == yexpected);
		QVERIFY(0.4 == m_e->get_time());
		QVERIFY(4 == m_e->get_step());
	}

}

void test_nbody_stream::negative_branch()
{
	{
		nbody_data_stream	stream;
		QVERIFY(0 != stream.write(NULL));
	}
	{
		nbody_data_stream	stream;
		nbody_engine_simple	e;
		QVERIFY(0 != stream.write(&e));
	}

	{
		nbody_data_stream	stream;
		QVERIFY(0 != stream.open("/nameless/file", 1000));
		QVERIFY(0 != stream.write(m_e));
	}

	{
		nbody_data_stream	stream;
		QVERIFY(0 != stream.write(m_e));
	}

	{
		nbody_data_stream	stream;
		QVERIFY(0 == stream.open("/tmp/nbody_test/00/stream", 1000));
		QVERIFY(0 == stream.open("/tmp/nbody_test/01/stream", 1000));
		QVERIFY(0 == stream.write(m_e));
	}

	{
		nbody_data_stream	stream;
		QDir("/tmp/nbody_test/0/stream0.dat").mkpath(".");
		QVERIFY(0 != stream.open("/tmp/nbody_test/0/stream", 1));
		QVERIFY(0 != stream.write(m_e));
		QDir("/tmp").rmpath("nbody_test/0");
	}
	{
		nbody_data_stream	stream;
		QDir("/tmp/nbody_test/1/stream.idx").mkpath(".");
		QVERIFY(0 != stream.open("/tmp/nbody_test/1/stream", 1));
		QVERIFY(0 != stream.write(m_e));
		QDir("/tmp").rmpath("nbody_test/1");
	}
	{
		nbody_data_stream	stream;
		QDir("/tmp/nbody_test/2/stream1.dat").mkpath(".");
		QVERIFY(0 == stream.open("/tmp/nbody_test/2/stream", 1));
		QVERIFY(0 == stream.write(m_e));
		QVERIFY(0 != stream.write(m_e));
		QDir("/tmp").rmpath("nbody_test/2");
	}

	{
		nbody_data_stream_reader	stream;
		QVERIFY(0 != stream.load("/nameless/stream"));
	}

	{
		nbody_data_stream_reader	stream;
		QDir("/tmp/nbody_test/3").mkpath(".");
		{
			QFile	f("/tmp/nbody_test/3/stream.idx");
			f.open(QFile::WriteOnly);
			f.write(QByteArray("0\t1\t2\n"));
		}
		QVERIFY(0 != stream.load("/tmp/nbody_test/3/stream"));
	}

	{
		nbody_data_stream_reader	stream;
		QDir("/tmp/nbody_test/4").mkpath(".");
		{
			QFile	f("/tmp/nbody_test/4/stream.idx");
			f.open(QFile::WriteOnly);
			f.write(QByteArray("a\tb\tc\td\n"));
		}
		QVERIFY(0 != stream.load("/tmp/nbody_test/4/stream"));
	}

	{
		nbody_data_stream_reader	stream;
		QVERIFY(0 == stream.get_frame_count());
		QVERIFY(0 == stream.get_steps_count());
		QVERIFY(0 == stream.get_max_time());
	}

	{
		{
			nbody_data_stream	stream;
			QVERIFY(0 == stream.open("/tmp/nbody_test/5/stream", 1));
			QVERIFY(0 == stream.write(m_e));
			QVERIFY(0 == stream.write(m_e));
			QFile::remove("/tmp/nbody_test/5/stream0.dat");
		}

		nbody_data_stream_reader	stream;
		QVERIFY(0 != stream.load("/tmp/nbody_test/5/stream"));
	}

	{
		{
			nbody_data_stream	stream;
			QVERIFY(0 == stream.open("/tmp/nbody_test/6/stream", 1));
			QVERIFY(0 == stream.write(m_e));
			QVERIFY(0 == stream.write(m_e));
			QFile::remove("/tmp/nbody_test/6/stream1.dat");
		}

		nbody_data_stream_reader	stream;
		QVERIFY(0 == stream.load("/tmp/nbody_test/6/stream"));
		QVERIFY(0 == stream.read(m_e));
		QVERIFY(0 != stream.read(m_e));
	}

	{
		{
			nbody_data_stream	stream;
			QVERIFY(0 == stream.open("/tmp/nbody_test/7/stream", 1));
			QVERIFY(0 == stream.write(m_e));
			QVERIFY(0 == stream.write(m_e));
			QVERIFY(0 == stream.write(m_e));
		}
		{
			QFile	f("/tmp/nbody_test/7/stream.idx");
			f.open(QFile::WriteOnly);
			f.write(QByteArray("1\t1\t0\t0\n"));
			f.write(QByteArray("1\t1\t1\t-1\n"));  //Negative file offset
			f.write(QByteArray("1\t1\t2\t-1\n"));  //Negative file offset
		}

		nbody_data_stream_reader	stream;
		QVERIFY(0 == stream.load("/tmp/nbody_test/7/stream"));
		QVERIFY(0 == stream.read(m_e));
		QVERIFY(0 != stream.read(m_e));
		QVERIFY(0 != stream.read(m_e));
	}


	{
		{
			nbody_data_stream	stream;
			QVERIFY(0 == stream.open("/tmp/nbody_test/8/stream", 1));
			QVERIFY(0 == stream.write(m_e));
		}

		nbody_data_stream_reader	stream;
		nbody_engine_simple			e0;
		QVERIFY(0 == stream.load("/tmp/nbody_test/8/stream"));
		QVERIFY(0 != stream.read(NULL));
		QVERIFY(0 != stream.read(&e0));
	}
}

int main(int argc, char* argv[])
{
	test_nbody_stream tc1;
	return QTest::qExec(&tc1, argc, argv);
}

#include "test_nbody_stream.moc"