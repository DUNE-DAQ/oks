#include "CLI/CLI.hpp"
#include <iostream>
#include "oks/kernel.hpp"
#include <fmt/core.h>
#include <deque>

namespace dunedaq {
namespace oks {


std::deque<std::set<OksClass*>> construct_class_domains(const OksKernel& k) {

  std::deque<std::set<OksClass*>> domains;

  // Find classes with no superclasses.
  // They are domain (or inheritance cluster) seeds
  std::deque<OksClass*> progenitors;
  for ( auto const& [key, kl] : k.classes()) {
      if (kl->direct_super_classes() == nullptr) {
          progenitors.push_back(kl);
      }
  }


  // Loop over seeds
  for( auto kl : progenitors ) {
    
    // Make a candidate cluster based using the seed subclasses
    std::set<OksClass*> klass_cluster;
    klass_cluster.insert(kl);
    klass_cluster.insert(kl->all_sub_classes()->begin(), kl->all_sub_classes()->end());
    
    // Look for overlaps with other domains
    std::deque<std::set<OksClass*>> overlapping;
    for( auto& dom : domains ) {
      std::set<OksClass*> intersection;
      std::set_intersection(dom.begin(), dom.end(), klass_cluster.begin(), klass_cluster.end(),
        std::inserter(intersection, intersection.begin()));
      // non-zero intersection, overlap found
      if (intersection.size() > 0) {
        overlapping.push_back(dom);
      }
    }

    // If overlapping are found, merge all overlapping domains
    if ( overlapping.size() > 0 ) {
      for( auto& dom : overlapping ) {
        // merge the existing cluster in klass_cluster
        klass_cluster.insert(dom.begin(), dom.end());
        // Remove the old cluster from the list
        auto it = std::find(domains.begin(), domains.end(), dom);
        if (it!= domains.end()) {
            domains.erase(it);
        }
      }
    }

    domains.push_back(klass_cluster);

  }
  
  return domains;
}

} // namespace oks
} // namespace dunedaq

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

    auto domains = construct_class_domains(k);

    std::map<const OksClass*, uint> class_domain_map;


    // Print the clustered domains
    fmt::print("Found {} inheritance domains\n", domains.size());
    for( size_t i(0); i<domains.size(); ++i ) {
      auto& d = domains[i];
      fmt::print("   - {} : [", i);
      for( const auto* kl : d ) {
        fmt::print(" {},", kl->get_name());
        class_domain_map[kl] = i;
      }
      fmt::print("]\n");
    }


  

    /* code */
    return 0;
}
