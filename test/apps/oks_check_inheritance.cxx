#include "CLI/CLI.hpp"
#include <iostream>
#include "oks/kernel.hpp"
#include <fmt/core.h>
#include <deque>


int main(int argc, char const *argv[])
{
    using namespace dunedaq::oks;

    CLI::App app{"App description"};

    std::string file;
    app.add_option("-f,--file", file, "Schema file")
        ->required()
        ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    fmt::print("Schema file: {}\n", file);

    OksKernel k;
    fmt::print("Kernel allow duplicated objects: {}\n", k.get_allow_duplicated_objects_mode());
    fmt::print("Kernel test duplicated objects via inheritance: {}\n", k.get_test_duplicated_objects_via_inheritance_mode());
    k.set_test_duplicated_objects_via_inheritance_mode(true);
    fmt::print("Kernel test duplicated objects via inheritance: {}\n", k.get_test_duplicated_objects_via_inheritance_mode());

    k.load_schema(file);

    std::deque<OksClass*> top_classes;
    for ( auto const& [key, kl] : k.classes()) {
        if (kl->direct_super_classes() == nullptr) {
            top_classes.push_back(kl);
        }
    }

    fmt::print("Initial cluster seeds\n");
    for( auto kl : top_classes ) {
        fmt::print("   {}\n", kl->get_name());
    }

    std::deque<std::set<OksClass*>> clusters;
    for( auto kl : top_classes ) {
        
      std::set<OksClass*> klass_cluster;
      klass_cluster.insert(kl);
      klass_cluster.insert(kl->all_sub_classes()->begin(), kl->all_sub_classes()->end());
      
      std::deque<std::set<OksClass*>> overlaps;
      for( auto& cl : clusters ) {
        std::set<OksClass*> intersection;
        std::set_intersection(cl.begin(), cl.end(), klass_cluster.begin(), klass_cluster.end(),
          std::inserter(intersection, intersection.begin()));
        if (intersection.size() > 0) {
          overlaps.push_back(cl);
        }
        
      }

      if ( overlaps.size() == 0 ) {
        clusters.push_back(klass_cluster);
      } else {

        for( auto& cl : overlaps ) {
          klass_cluster.insert(cl.begin(), cl.end());
          auto it = std::find(clusters.begin(), clusters.end(), cl);
          if (it!= clusters.end()) {
              clusters.erase(it);
          }
        }

        clusters.push_back(klass_cluster);
      }
    }

    fmt::print("Found {} inheritance clusters\n", clusters.size());
    for( size_t i(0); i<clusters.size(); ++i ) {
      auto& cl = clusters[i];
      fmt::print("   - {} : [", i);
      for( const auto* kl : cl ) {
        fmt::print(" {},", kl->get_name());
      }
      fmt::print("]\n");
    }


    /* code */
    return 0;
}
