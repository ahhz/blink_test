#include <gtest/gtest.h>

#include <blink/raster/utility.h>
#include <blink/raster/gdal_raster_view.h>
#include <blink/raster/edge_view.h>
#include <blink/raster/gdal_input_iterator.h>
#include <boost/filesystem.hpp>

bool test_create_temp_gdal_raster()
{
  auto r = blink::raster::create_temp_gdal_raster<int>(5, 3);
  int count = 0;
  for (auto&& i : r)
  {
    i = count++;
  }
  std::vector<int> check_vector;
  for (auto&& i : r)
  {
    check_vector.push_back(i);
  }
  return check_vector == std::vector<int>
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
}

bool test_create_gdal_raster()
{
  {
    auto r = blink::raster::create_gdal_raster<int>("temp.tif", 5, 3);
    int count = 0;
    for (auto&& i : r)
    {
      i = count++;
    }
  } // leave scope
  bool check_exist = boost::filesystem::exists("temp.tif");
  boost::filesystem::remove("temp.tif");
  bool check_not_exist = !boost::filesystem::exists("temp.tif");
  return check_exist && check_not_exist;
}

bool test_open_gdal_raster()
{
  {
    auto r = blink::raster::create_gdal_raster<int>("temp.tif", 5, 3);
    int count = 0;
    for (auto&& i : r)
    {
      i = count++;
    }
  } // leave scope
  bool check_exist = boost::filesystem::exists("temp.tif");
  bool check_contents;
  {
    auto r = blink::raster::open_gdal_raster<int>("temp.tif", GA_ReadOnly);
    std::vector<int> check_vector;
    for (auto&& i : r)
    {
      check_vector.push_back(i);
    }
    check_contents =  check_vector == std::vector<int>
     {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  }
  boost::filesystem::remove("temp.tif");
  bool check_not_exist = !boost::filesystem::exists("temp.tif");
  return check_exist && check_not_exist && check_contents;
}

bool test_gdal_raster_delete_on_close()
{
  {
    auto r = blink::raster::create_gdal_raster<int>("temp2.tif", 5, 3);
    int count = 0;
    for (auto&& i : r)
    {
      i = count++;
    }
    r.set_delete_on_close(true);
  } // leave scope, remove temp.tif
  bool check_not_exist = !boost::filesystem::exists("temp2.tif");
  return check_not_exist;
}

bool test_gdal_raster_transpose_view()
{
  auto r = blink::raster::create_temp_gdal_raster<int>(3,5);
  int count = 0;
  for (auto&& i : r)
  {
    i = count++;
  }

  auto t = blink::raster::make_raster_view(
    &r, blink::raster::orientation::col_major{});
  std::vector<int> check_vector;
  for (auto&& i : t)
  {
    check_vector.push_back(i);
  }
  return check_vector == std::vector<int>
    {0, 5, 10, 1, 6, 11, 2, 7, 12, 3, 8, 13, 4, 9, 14};
  
}

bool test_gdal_raster_v_edge_view()
{
  auto r = blink::raster::create_temp_gdal_raster<int>(3, 5);
  int count = 0;
  for (auto&& i : r)
  {
    i = count++;
  }

  auto t = blink::raster::make_raster_view(
    &r, blink::raster::orientation::row_major{}, 
    blink::raster::element::v_edge{});
  
  std::vector < std::pair<boost::optional<int>, boost::optional<int> > > 
    check_vector;
  for (auto&& i : t)
  {
    check_vector.push_back(i);
  }
  using o_int = boost::optional<int>;
  using o_pair = std::pair<o_int, o_int>;
  
  std::vector < o_pair > c2;
  c2.push_back(o_pair(o_int(  ), o_int( 0)));
  c2.push_back(o_pair(o_int( 0), o_int( 1)));
  c2.push_back(o_pair(o_int( 1), o_int( 2)));
  c2.push_back(o_pair(o_int( 2), o_int( 3)));
  c2.push_back(o_pair(o_int( 3), o_int( 4)));
  c2.push_back(o_pair(o_int( 4), o_int(  )));
  c2.push_back(o_pair(o_int(  ), o_int( 5)));
  c2.push_back(o_pair(o_int( 5), o_int( 6)));
  c2.push_back(o_pair(o_int( 6), o_int( 7)));
  c2.push_back(o_pair(o_int( 7), o_int( 8)));
  c2.push_back(o_pair(o_int( 8), o_int( 9)));
  c2.push_back(o_pair(o_int( 9), o_int(  )));
  c2.push_back(o_pair(o_int(  ), o_int(10)));
  c2.push_back(o_pair(o_int(10), o_int(11)));
  c2.push_back(o_pair(o_int(11), o_int(12)));
  c2.push_back(o_pair(o_int(12), o_int(13)));
  c2.push_back(o_pair(o_int(13), o_int(14)));
  c2.push_back(o_pair(o_int(14), o_int(  )));

  return c2==check_vector;
}

bool test_gdal_raster_transpose_v_edge_view()
{
  auto r = blink::raster::create_temp_gdal_raster<int>(3, 5);
  int count = 0;
  for (auto&& i : r)
  {
    i = count++;
  }

  auto t = blink::raster::make_raster_view(
    &r, blink::raster::orientation::col_major{},
    blink::raster::element::v_edge{});

  std::vector < std::pair<boost::optional<int>, boost::optional<int> > >
    check_vector;
  for (auto&& i : t)
  {
    check_vector.push_back(i);
  }
  using o_int = boost::optional<int>;
  using o_pair = std::pair<o_int, o_int>;

  std::vector < o_pair > c2;
  c2.push_back(o_pair(o_int(), o_int(0)));
  c2.push_back(o_pair(o_int(), o_int(5)));
  c2.push_back(o_pair(o_int(), o_int(10)));
  c2.push_back(o_pair(o_int(0), o_int(1)));
  c2.push_back(o_pair(o_int(5), o_int(6)));
  c2.push_back(o_pair(o_int(10), o_int(11)));
  c2.push_back(o_pair(o_int(1), o_int(2)));
  c2.push_back(o_pair(o_int(6), o_int(7)));
  c2.push_back(o_pair(o_int(11), o_int(12)));
  c2.push_back(o_pair(o_int(2), o_int(3)));
  c2.push_back(o_pair(o_int(7), o_int(8)));
  c2.push_back(o_pair(o_int(12), o_int(13)));
  c2.push_back(o_pair(o_int(3), o_int(4)));
  c2.push_back(o_pair(o_int(8), o_int(9)));
  c2.push_back(o_pair(o_int(13), o_int(14)));
  c2.push_back(o_pair(o_int(4), o_int()));
  c2.push_back(o_pair(o_int(9), o_int()));
  c2.push_back(o_pair(o_int(14), o_int()));

  return c2 == check_vector;
}


bool test_gdal_raster_h_edge_view()
{
  auto r = blink::raster::create_temp_gdal_raster<int>(3, 5);
  int count = 0;
  for (auto&& i : r)
  {
    i = count++;
  }

  auto t = blink::raster::make_raster_view(
    &r, blink::raster::orientation::row_major{},
    blink::raster::element::h_edge{});

  std::vector < std::pair<boost::optional<int>, boost::optional<int> > >
    check_vector;
  for (auto&& i : t)
  {
    check_vector.push_back(i);
  }
  using o_int = boost::optional<int>;
  using o_pair = std::pair<o_int, o_int>;

  std::vector < o_pair > c2;
  c2.push_back(o_pair(o_int(), o_int(0)));
  c2.push_back(o_pair(o_int(), o_int(1)));
  c2.push_back(o_pair(o_int(), o_int(2)));
  c2.push_back(o_pair(o_int(), o_int(3)));
  c2.push_back(o_pair(o_int(), o_int(4)));
  c2.push_back(o_pair(o_int(0), o_int(5)));
  c2.push_back(o_pair(o_int(1), o_int(6)));
  c2.push_back(o_pair(o_int(2), o_int(7)));
  c2.push_back(o_pair(o_int(3), o_int(8)));
  c2.push_back(o_pair(o_int(4), o_int(9)));
  c2.push_back(o_pair(o_int(5), o_int(10)));
  c2.push_back(o_pair(o_int(6), o_int(11)));
  c2.push_back(o_pair(o_int(7), o_int(12)));
  c2.push_back(o_pair(o_int(8), o_int(13)));
  c2.push_back(o_pair(o_int(9), o_int(14)));
  c2.push_back(o_pair(o_int(10), o_int()));
  c2.push_back(o_pair(o_int(11), o_int()));
  c2.push_back(o_pair(o_int(12), o_int()));
  c2.push_back(o_pair(o_int(13), o_int()));
  c2.push_back(o_pair(o_int(14), o_int()));

  return c2 == check_vector;
}

bool test_gdal_raster_transpose_h_edge_view()
{
  auto r = blink::raster::create_temp_gdal_raster<int>(3, 5);
  int count = 0;
  for (auto&& i : r)
  {
    i = count++;
  }

  auto t = blink::raster::make_raster_view(
    &r, blink::raster::orientation::col_major{},
    blink::raster::element::h_edge{});

  std::vector < std::pair<boost::optional<int>, boost::optional<int> > >
    check_vector;
  for (auto&& i : t)
  {
    check_vector.push_back(i);
  }
  using o_int = boost::optional<int>;
  using o_pair = std::pair<o_int, o_int>;

  std::vector < o_pair > c2;
  
  c2.push_back(o_pair(o_int(), o_int(0)));
  c2.push_back(o_pair(o_int(0), o_int(5)));
  c2.push_back(o_pair(o_int(5), o_int(10)));
  c2.push_back(o_pair(o_int(10), o_int()));

  c2.push_back(o_pair(o_int(), o_int(1)));
  c2.push_back(o_pair(o_int(1), o_int(6)));
  c2.push_back(o_pair(o_int(6), o_int(11)));
  c2.push_back(o_pair(o_int(11), o_int()));

  c2.push_back(o_pair(o_int(), o_int(2)));
  c2.push_back(o_pair(o_int(2), o_int(7)));
  c2.push_back(o_pair(o_int(7), o_int(12)));
  c2.push_back(o_pair(o_int(12), o_int()));

  c2.push_back(o_pair(o_int(), o_int(3)));
  c2.push_back(o_pair(o_int(3), o_int(8)));
  c2.push_back(o_pair(o_int(8), o_int(13)));
  c2.push_back(o_pair(o_int(13), o_int()));

  c2.push_back(o_pair(o_int(), o_int(4)));
  c2.push_back(o_pair(o_int(4), o_int(9)));
  c2.push_back(o_pair(o_int(9), o_int(14)));
  c2.push_back(o_pair(o_int(14), o_int()));

  return c2 == check_vector;
}

bool test_input_view_raster()
{
  {
    auto r = blink::raster::create_gdal_raster<int>("temp.tif", 5, 3);
    int count = 0;
    for (auto&& i : r)
    {
      i = count++;
    }
  } // leave scope
  bool check_exist = boost::filesystem::exists("temp.tif");
  bool check_contents;
  {
    auto band = blink::raster::detail::gdal_makers::open_band("temp.tif", GA_ReadOnly);
    blink::raster::gdalrasterband_input_view<const int> view(band);
    std::vector<int> check_vector;
   
    for (auto&& i : view)
    {
      check_vector.push_back(i);
    }
   
    check_contents = check_vector == std::vector<int>
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  }
  boost::filesystem::remove("temp.tif");
  bool check_not_exist = !boost::filesystem::exists("temp.tif");
  return check_exist && check_not_exist && check_contents;
}

bool test_gdal_raster_large()
{
  {
    auto view = blink::raster::create_gdal_raster<int>("temp.tif", 5000, 3000);
    int count = 0;
    for (auto&& i : view)
    {
      i = count++;
    }
  }
  bool check_exist = boost::filesystem::exists("temp.tif");

  bool check_contents;
  {
    auto view = blink::raster::open_gdal_raster<int>("temp.tif", GA_ReadOnly);

    std::vector<int> check_vector;
    for (auto&& i : view)
    {
      check_vector.push_back(i);
    }
    int last = check_vector.back();
    check_vector.resize(15);
    check_contents = check_vector == std::vector<int>
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}
    && last == (5000 * 3000 - 1);
  }
  boost::filesystem::remove("temp.tif");
  bool check_not_exist = !boost::filesystem::exists("temp.tif");
  return check_exist && check_not_exist && check_contents;
}


bool test_input_view_raster_large()
{
  int rows = 50;
  int cols = 30;
  {
    auto band = blink::raster::detail::gdal_makers::create_band("temp.tif", rows, cols, 
      GDT_Int32);
    blink::raster::gdalrasterband_input_view<int> view(band);
  
    int count = 0;
    for (auto&& i : view)
    {
      i = count++;
    }
  }
  bool check_exist = boost::filesystem::exists("temp.tif");

  bool check_contents;
  {
    auto band = blink::raster::detail::gdal_makers::open_band("temp.tif",
      //GA_Update,
      GA_ReadOnly,
      1);
    blink::raster::gdalrasterband_input_view<const int> view(band);
    
    std::vector<int> check_vector;
    for (auto&& i : view)
    {
      check_vector.push_back(i);
    }
   
    int last = check_vector.back();
    check_vector.resize(15);
    check_contents = check_vector == std::vector<int>
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}
    && last == (rows*cols  - 1);
  }
  boost::filesystem::remove("temp.tif");
  bool check_not_exist = !boost::filesystem::exists("temp.tif");
  return check_exist && check_not_exist && check_contents;
}

#if NDEBUG // Ouch, have not build gdal libraries in debug and now get funny errors
TEST(Raster, GDALRaster) {
  EXPECT_TRUE(test_create_temp_gdal_raster());
  EXPECT_TRUE(test_create_gdal_raster());
  EXPECT_TRUE(test_open_gdal_raster());
  EXPECT_TRUE(test_gdal_raster_delete_on_close());
  EXPECT_TRUE(test_gdal_raster_transpose_view());
  EXPECT_TRUE(test_gdal_raster_v_edge_view());
  EXPECT_TRUE(test_gdal_raster_transpose_v_edge_view());
  EXPECT_TRUE(test_gdal_raster_h_edge_view());
  EXPECT_TRUE(test_gdal_raster_transpose_h_edge_view());
  EXPECT_TRUE(test_input_view_raster());
  EXPECT_TRUE(test_input_view_raster_large());
 // EXPECT_TRUE(test_gdal_raster_large()); 
 
 }
#endif
