#include <obs-module.h>
#include <graphics/image-file.h>
#include <util/platform.h>
#include <util/dstr.h>
#include <sys/stat.h>
#include <stdio.h>
#include <obs.h>
#include <lsl_c.h>
#include <stdio.h>
#include <lsl_cpp.h>
#include <vector>
#include <obs.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <map>
#include <thread>
#include <algorithm>


#define S_HASTEXT "link"

#define T_HASTEXT "Enable link"

#define SETTING_LSL_CHAN              "LSL_chan"

#define TEXT_LSL_CHAN                obs_module_text("LSL_chan")

#define SETTING_NPOINTS              "N_Points"

#define TEXT_NPOINTS               obs_module_text("N_Points")

#define blog(log_level, format, ...)                    \
	blog(log_level, "[graphe_source: '%s'] " format, \
	     obs_source_get_name(context->source), ##__VA_ARGS__)

#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)

struct graphe_source {
	obs_source_t *source;

	char *file;
	bool persistent;
	time_t file_timestamp;
	float update_time_elapsed;
	uint64_t last_time;
	bool active;
	double x[100];
	double y[100];
	double x_50[50];
	double y_50[50];
	double x_25[25];
	double y_25[25];
	double x_10[10];
	double y_10[10];
	QwtPlot *myplot;
	QwtPlotCurve *curve;
		const char *lsl_chan_name;
		int lsl_Npoints;
		bool link;


	uint32_t cx=100,cy=100;
	// if2;
};

static time_t get_modified_timestamp(const char *filename)
{
	struct stat stats;
	if (os_stat(filename, &stats) != 0)
		return -1;
	return stats.st_mtime;
}

static const char *graphe_source_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "Hbeat_graphe";//obs_module_text("ImageInput");
}





static void graphe_source_update(void *data, obs_data_t *settings)
{
	 graphe_source* context = (graphe_source*)data;


	 context->lsl_chan_name =
	 	obs_data_get_string(settings, SETTING_LSL_CHAN);


		context->lsl_Npoints =
 	 	obs_data_get_int(settings, SETTING_NPOINTS);

			context->link =obs_data_get_bool(settings, S_HASTEXT);








}

static void graphe_source_defaults(obs_data_t *settings)
{
	//obs_data_set_default_bool(settings, "unload", false);
}



static void *graphe_source_create(obs_data_t *settings, obs_source_t *source)
{
	struct graphe_source* context = (graphe_source*) bzalloc(sizeof(struct graphe_source));
	//initialisation du graphe
	context->source = source;
	context->persistent = true;
	context->myplot = new QwtPlot;


	context->curve =  new QwtPlotCurve("curve1");

	context->myplot->show();


//initialisation des différent graphes a 0
	for (int i = 0;i < 100;i++){
		context->x[i] = i;
		context->y[i] = i;

	}

	for (int i = 0;i < 50;i++){
		context->x_50[i] = i;
		context->y_50[i] = i;

	}

	for (int i = 0;i < 25;i++){
		context->x_25[i] = i;
		context->y_25[i] = i;

	}

	for (int i = 0;i < 10;i++){
		context->x_10[i] = i;
		context->y_10[i] = i;



	}
 //liaison des valeurs et affichache du graphe
	context->curve->setSamples(context->x,context->y,100);
	context->curve->attach(context->myplot);
	context->myplot->replot();







	graphe_source_update(context, settings);
	return context;
}

static void graphe_source_destroy(void *data)
{
	graphe_source* context = (graphe_source*)data;


	bfree(context);
}

static uint32_t graphe_source_getwidth(void *data)
{
	graphe_source* context = (graphe_source*)data;
	return context-> cx;
}

static uint32_t graphe_source_getheight(void *data)
{
	graphe_source* context = (graphe_source*)data;
	return context-> cy;
}

static void graphe_source_render(void *data, gs_effect_t *effect)
{
	graphe_source* context = (graphe_source*)data;

	context->myplot->show();





}

static void graphe_source_tick(void *data, float seconds)
{
	graphe_source* context = (graphe_source*)data;
	uint64_t frame_time = obs_get_video_frame_time();

	context->update_time_elapsed += seconds;

	if (obs_source_showing(context->source)) {
		if (context->update_time_elapsed >= 1.0f) {
			time_t t = get_modified_timestamp(context->file);
			context->update_time_elapsed = 0.0f;

			if (context->file_timestamp != t) {
				//graphe_source_load(context);
			}
		}
	}

if(context->link){

		std::vector<lsl::stream_info> results = lsl::resolve_streams();

		std::map<std::string, lsl::stream_info> found_streams;
		int i;
		i = 0;
		int a;
		for (auto &stream : results) {
			found_streams.emplace(std::make_pair(stream.uid(), stream));
			if (strcmp(context->lsl_chan_name, stream.name().c_str()) == 0)
			{

				using namespace lsl;

					stream_inlet inlet(results[i]);

					std::vector<int> simple;

					inlet.pull_sample(simple);
					a =  simple.front();

			}
			i = i +1;
		}

		//mise a jour des tasbleau en fonctio du nombre de points choisits
		if(context->lsl_Npoints == 50){


			for (int k = 0;k < 49;k++){

				context->y_50[k] = 	context->y_50[k+1];

			}

			context->y_50[49] = a;
		//atache les nouvelle valeur au graphe
		context->curve->setSamples(context->x_50,context->y_50,50);
		context->curve->attach(context->myplot);
		context->myplot->replot();




		}

		if(context->lsl_Npoints == 25){


			for (int k = 0;k < 24;k++){

				context->y_25[k] = 	context->y_25[k+1];

			}

			context->y_25[24] = a;

		context->curve->setSamples(context->x_25,context->y_25,25);
		context->curve->attach(context->myplot);
		context->myplot->replot();




		}
		if(context->lsl_Npoints == 10){


			for (int k = 0;k < 9;k++){

				context->y_10[k] = 	context->y_10[k+1];

			}

			context->y_10[9] = a;

		context->curve->setSamples(context->x_10,context->y_10,10);
		context->curve->attach(context->myplot);
		context->myplot->replot();




		}
		if(context->lsl_Npoints == 100){

		for (int k = 0;k < 99;k++){

			context->y[k] = 	context->y[k+1];

		}

		context->y[99] = a;

	context->curve->setSamples(context->x,context->y,100);
	context->curve->attach(context->myplot);
	context->myplot->replot();
}


}






	context->last_time = frame_time;
}



static obs_properties_t *graphe_source_properties(void *data)
{
	 graphe_source* s = (graphe_source*)data;


	obs_properties_t *props = obs_properties_create();
	obs_properties_add_bool(props, S_HASTEXT, T_HASTEXT);

	obs_property_t *p = obs_properties_add_list(props, SETTING_LSL_CHAN,
								TEXT_LSL_CHAN,
								OBS_COMBO_TYPE_LIST,
								OBS_COMBO_FORMAT_STRING);

	std::vector<lsl::stream_info> results = lsl::resolve_streams();

	std::map<std::string, lsl::stream_info> found_streams;
								// display them
	for (auto &stream : results) {
				found_streams.emplace(std::make_pair(stream.uid(), stream));
					 //lsl_name = stream.name();

	obs_property_list_add_string(p, stream.name().c_str(), stream.name().c_str());
	}
//combobox de INT
	obs_property_t *n = obs_properties_add_list(props, SETTING_NPOINTS,
								TEXT_NPOINTS,
								OBS_COMBO_TYPE_LIST,
								OBS_COMBO_FORMAT_INT);

	obs_property_list_add_int(n, "10", 10);
	obs_property_list_add_int(n, "25", 25);
	obs_property_list_add_int(n, "50", 50);
	obs_property_list_add_int(n, "100", 100);




	return props;
}








 struct obs_source_info create_plugin_info() {
	 struct obs_source_info plugin_info = {};
	plugin_info.id = "Hbeat_graphe";
	plugin_info.type = OBS_SOURCE_TYPE_INPUT;
	plugin_info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;
	plugin_info.get_name = graphe_source_get_name;
	plugin_info.create = graphe_source_create;
	plugin_info.destroy = graphe_source_destroy;
	plugin_info.update = graphe_source_update;
	plugin_info.get_defaults = graphe_source_defaults;
	plugin_info.get_width = graphe_source_getwidth;
	plugin_info.get_height = graphe_source_getheight;
	plugin_info.video_render = graphe_source_render;
	plugin_info.video_tick = graphe_source_tick;

	plugin_info.get_properties = graphe_source_properties;
	//plugin_info.icon_type = OBS_ICON_TYPE_IMAGE;
	return plugin_info;
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("graphe-source", "en-US")
MODULE_EXPORT const char *obs_module_description(void)
{
	return "graphe_lsl";
}



bool obs_module_load(void)
{
	obs_source_info Hbeat_image_info = create_plugin_info();
	obs_register_source(&Hbeat_image_info);

	return true;
}
