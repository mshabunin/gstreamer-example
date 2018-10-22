#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <thread>
#include <math.h>

using namespace std;

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>
#include <gst/app/gstappsink.h>


inline void sleepForSec(int sec)
{
    cout << "Sleeping for " << sec << " sec..." << endl;
    this_thread::sleep_for(chrono::seconds(sec));
}

//inline bool hasElement(const gchar * name)
//{
//    GstElementFactory * fac = gst_element_factory_find(name);
//    if (!fac)
//        return false;
//    gst_object_unref(GST_OBJECT(fac));
//    return true;
//}

//static GstBusSyncReply busHandler(GstBus * /*bus*/, GstMessage * message, gpointer /*user_data*/)
//{
//    cout << "MSG : " << GST_MESSAGE_TYPE_NAME(message);
//    switch (GST_MESSAGE_TYPE(message))
//    {
//    case GST_MESSAGE_ERROR:
//    {
//        GError *err;
//        gchar *debug;
//        gst_message_parse_error(message, &err, &debug);
//        cout << " : " << GST_OBJECT_NAME(message->src) << " : " << err->message << endl;
//        if (debug)
//            cout << "    : " << debug << endl;
//        g_error_free(err);
//        g_free(debug);
//        break;
//    }
//    case GST_MESSAGE_STATE_CHANGED:
//    {
//        GstState a, b, c;
//        gst_message_parse_state_changed(message, &a, &b, &c);
//        cout << " : " << a << " -> " << b << " (pending " << c << ")" << endl;
//        break;
//    }
//    default: cout << endl; break;
//    }
//    return GST_BUS_PASS;
//}

//static gboolean busHandler(GstBus * /*bus*/, GstMessage * message, gpointer)
//{
//    cout << "MSG : " << GST_MESSAGE_TIMESTAMP(message) << " : " << GST_MESSAGE_TYPE_NAME(message) << endl;
//    return TRUE;
//}

typedef gboolean (*QueryFun)(GstElement *, GstFormat, gint64 *);

static void printOne(const GValue *item, gpointer ptr)
{
    QueryFun fun = (QueryFun)ptr;
    GstElement * elem = GST_ELEMENT(g_value_get_object(item));
    gint64 f = -1, t = -1;
    gboolean res;

    cout << setw(20) << GST_ELEMENT_NAME(elem);

    res = fun(elem, GST_FORMAT_DEFAULT, &f);
    cout << ", " << " => frame ";
    if (res) cout << f; else cout << "?";

    res = fun(elem, GST_FORMAT_TIME, &t);
    cout << ", " << " => time ";
    if (res) cout << t; else cout << "?";

    cout << endl;
}

static void printPipeline(GstPipeline * pipeline, QueryFun fun, const string &nm)
{
    cout << nm << endl;
    GstIterator * it = gst_bin_iterate_recurse(GST_BIN(pipeline));
    gst_iterator_foreach(it, printOne, (gpointer)fun);
    gst_iterator_free(it);
}

static void printSample(GstSample * sample)
{
    cout << "Sample: " << sample;

//    const GstSegment * seg = gst_sample_get_segment(sample);
//    cout << "  segment: " << seg << endl;
//    if (seg)
//    {
//        cout << "    Flags: " << seg->flags << endl;
//        cout << "    Rate: " << seg->rate << endl;
//        cout << "    Applied rate: " << seg->applied_rate << endl;
//        cout << "    Format: " << seg->format << endl;
//        cout << "    Base: " << seg->base << endl;
//        cout << "    Offset: " << seg->offset << endl;
//        cout << "    Start: " << seg->start << endl;
//        cout << "    Stop: " << seg->stop << endl;
//        cout << "    Time: " << seg->time << endl;
//        cout << "    Position: " << seg->position << endl;
//        cout << "    Duration: " << seg->duration << endl;
//    }

//    const GstStructure * info = gst_sample_get_info(sample);
//    cout << "  info: " << info << endl;
//    if (info)
//    {
//        gchar * str = gst_structure_to_string(info);
//        cout << "    " << str << endl;
//        g_free(str);
//    }

    GstBuffer * buf = gst_sample_get_buffer(sample);
    if (buf)
    {
        GstMapInfo info;
        gst_buffer_map(buf, &info, GST_MAP_READ);
        uint hash = 0;
        for (size_t i = 0; i < info.size; ++i)
            hash ^= info.data[i];
        cout << "  Hash: " << hash << ", Size: " << info.size << endl;
        gst_buffer_unmap(buf, &info);
    }

//    GstCaps * caps = gst_sample_get_caps(sample);
//    cout << "  caps: " << caps << endl;
//    if (caps)
//    {
//        gchar * str = gst_caps_to_string(caps);
//        cout << "    " << str << endl;
//        g_free(str);
//    }


//    GstBuffer * buf = gst_sample_get_buffer(sample);
//    cout << "  buf: " << buf << endl;
//    if (buf)
//    {
//        cout << "    size: " << gst_buffer_get_size(buf) << endl;
//        guint mems = gst_buffer_n_memory(buf);
//        for (guint i = 0; i < mems; ++i)
//        {
//            GstMemory * mem = gst_buffer_peek_memory(buf, i);
//            guint f = GST_MEMORY_FLAGS(mem);
//            cout << "    mem " << i << ": " << mem << " / " << gst_object_get_name(GST_OBJECT(mem->allocator)) << endl;
//            cout << "      GST_MEMORY_FLAG_READONLY: " << (f & GST_MEMORY_FLAG_READONLY ? "YES" : "NO") << endl;
//            cout << "      GST_MEMORY_FLAG_NO_SHARE: " << (f & GST_MEMORY_FLAG_NO_SHARE ? "YES" : "NO") << endl;
//            cout << "      GST_MEMORY_FLAG_ZERO_PREFIXED: " << (f & GST_MEMORY_FLAG_ZERO_PREFIXED ? "YES" : "NO") << endl;
//            cout << "      GST_MEMORY_FLAG_ZERO_PADDED: " << (f & GST_MEMORY_FLAG_ZERO_PADDED ? "YES" : "NO") << endl;
//            cout << "      GST_MEMORY_FLAG_PHYSICALLY_CONTIGUOUS: " << (f & GST_MEMORY_FLAG_PHYSICALLY_CONTIGUOUS ? "YES" : "NO") << endl;
//            cout << "      GST_MEMORY_FLAG_NOT_MAPPABLE: " << (f & GST_MEMORY_FLAG_NOT_MAPPABLE ? "YES" : "NO") << endl;
//        }
//    }
}

inline string gstStateName(GstState s)
{
    switch (s)
    {
    case GST_STATE_VOID_PENDING: return "GST_STATE_VOID_PENDING";
    case GST_STATE_NULL: return "GST_STATE_NULL";
    case GST_STATE_READY: return "GST_STATE_READY";
    case GST_STATE_PAUSED: return "GST_STATE_PAUSED";
    case GST_STATE_PLAYING: return "GST_STATE_PLAYING";
    }
    return "invalid";
}

inline bool changeState(GstPipeline *pipeline, GstState state)
{
    GstStateChangeReturn res = gst_element_set_state(GST_ELEMENT(pipeline), state);
    if (res == GST_STATE_CHANGE_ASYNC)
        res =  gst_element_get_state(GST_ELEMENT(pipeline), NULL, NULL, GST_CLOCK_TIME_NONE);
    if (res != GST_STATE_CHANGE_SUCCESS)
    {
        cerr << "Failed to go to state " << gstStateName(state) << ": " << res << endl;
        return false;
    }
    cout << "State changed to " << gstStateName(state) << endl;
    return true;
}

int main(int argc, const char * argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: <app> <filename>" << endl;
        return -8;
    }
    gst_init(NULL, NULL);

    guint major, minor, micro, nano;
    gst_version(&major, &minor, &micro, &nano);
    cout << "GStreamer runtime version: " << major << "." << minor << "." << micro << "." << nano << endl;
    cout << "GStreamer build version: " << GST_VERSION_MAJOR << "." << GST_VERSION_MINOR << "." << GST_VERSION_MICRO << "." << GST_VERSION_NANO << endl;

//    string desc = "v4l2src ! video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! appsink sync=false name=sink";
    const string filename(argv[1]);
    cout << "Filename: " << filename << endl;
    stringstream p;
    p << "uridecodebin uri=file://" << filename << " ! "
      << "appsink name=sink sync=false async=false drop=false max-buffers=1 emit-signals=false wait-on-eos=false";
    cout << "Pipeline: " << p.str() << endl;

    GError * err = NULL;
    GstElement * parsed = gst_parse_launch(p.str().c_str(), &err);
    if (!parsed)
    {
        cout << "Failed to parse: " << err->message << endl;
        return -1;
    }
    else if (err)
    {
        cout << "Recoverable error: " << err->message << endl;
    }

    GstPipeline * pipeline = GST_PIPELINE(parsed);
    GstElement * sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
    if (!pipeline || !sink || !GST_IS_PIPELINE(pipeline) || !GST_IS_APP_SINK(sink))
    {
        cout << "Bad pipeline or sink!" << endl;
        return 5;
    }

    gst_pipeline_use_clock(GST_PIPELINE(pipeline), NULL);

    cout << "SINK options" << endl << boolalpha;
    cout << "  sync: " << (bool)gst_base_sink_get_sync(GST_BASE_SINK(sink)) << endl;

    cout << "  max lateness: " << gst_base_sink_get_max_lateness(GST_BASE_SINK(sink)) << endl;
    cout << "  qos: " << (bool)gst_base_sink_is_qos_enabled(GST_BASE_SINK(sink)) << endl;
    cout << "  async: " << (bool)gst_base_sink_is_async_enabled(GST_BASE_SINK(sink)) << endl;
    cout << "  tuning sync: " << gst_base_sink_get_ts_offset(GST_BASE_SINK(sink)) << endl;
    cout << "  latency: " << gst_base_sink_get_latency(GST_BASE_SINK(sink)) << endl;

    cout << "  drop: " << (bool)gst_app_sink_get_drop(GST_APP_SINK(sink)) << endl;
    cout << "  wait on EOS: " << (bool)gst_app_sink_get_wait_on_eos(GST_APP_SINK(sink)) << endl;
    cout << "  max buffers: " << gst_app_sink_get_max_buffers(GST_APP_SINK(sink)) << endl;

    if (!changeState(pipeline, GST_STATE_PAUSED))
        return 6;

    printPipeline(pipeline, gst_element_query_duration, "Duration");

    if (!changeState(pipeline, GST_STATE_PLAYING))
        return 6;

    {
        GstClock * clock = gst_element_get_clock(GST_ELEMENT(pipeline));
        if (!clock)
        {
            cout << "No clock!" << endl;
        }
        else
        {
            cout << "Resolution: " << GST_TIME_AS_NSECONDS(gst_clock_get_resolution(clock)) << " ns" << endl;
            for (int j = 0; j < 10; ++j)
                cout << "Time: " << GST_TIME_AS_NSECONDS(gst_clock_get_time(clock)) << " ns" << endl;
            gst_object_unref(clock);
        }
    }

    double ns_per_frame = 0;
    {
        gint64 f_time = -1, f_frames = -1;
        if (gst_element_query_duration(GST_ELEMENT(sink), GST_FORMAT_TIME, &f_time) &&
            gst_element_query_duration(GST_ELEMENT(sink), GST_FORMAT_DEFAULT, &f_frames))
        {
            ns_per_frame = (double)f_time / (double)f_frames;
            cout << "ns_per_frame: " << ns_per_frame << endl;
        }
        else
        {
            cout << "ns_per_frame to be detected..." << endl;
        }
    }

    {
        gint64 prev_time = -1;
        int idx = -1;
        while (true)
        {
            ++idx;
            cout << "*** " << idx << endl;
            gboolean eos = gst_app_sink_is_eos(GST_APP_SINK(sink));
            if (eos)
            {
                cout << "EOS!" << endl;
                break;
            }

            {
                gint64 f_time = -1;
                gst_element_query_position(GST_ELEMENT(sink), GST_FORMAT_TIME, &f_time);
                if (ns_per_frame != 0)
                {
                    long pos = lrint(f_time / ns_per_frame) - 1;
                    long diff = idx - pos;
                    cout << "Position: " << f_time / 1000000. << " ms ==> Frame " << pos << " (diff " << diff  << ")" << (diff > 0 ? " <-------------" : "") <<  endl;
                }
//                if (ns_per_frame == 0 && prev_time == -1)
//                {
//                    ns_per_frame = f_time - prev_time;
//                    cout << "ns_per_frame set to " << ns_per_frame << endl;
//                }
//                prev_time = f_time;
            }

            GstSample * sample = gst_app_sink_try_pull_sample(GST_APP_SINK(sink), 5 * GST_SECOND);
            if (!sample)
            {
                cout << "Bad sample" << endl;
                continue;
            }
//            printSample(sample);
            gst_sample_unref(sample);

        }
    }

    changeState(pipeline, GST_STATE_NULL);

    gst_object_unref(GST_OBJECT(pipeline));
    gst_deinit();
    return 0;
}

