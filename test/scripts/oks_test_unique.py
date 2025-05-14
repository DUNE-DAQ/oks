#!/usr/bin/env python

import rich

from rich import print
import click
import oks


def cli():

    k = oks.OksKernel()
    print("allow duplicated objs mode", k.get_allow_duplicated_objects_mode())
    print("test duplicated objs mode", k.get_test_duplicated_objects_via_inheritance_mode())
    k.set_test_duplicated_objects_via_inheritance_mode(True)
    print("test duplicated objs mode", k.get_test_duplicated_objects_via_inheritance_mode())

    print('Loaded objects')



    k.load_data("dummyschema/dummydb.data.xml")

    top_classes = [ kl for kl in k.classes().values() if kl.direct_super_classes() is None ]

    print("Classes with no superclasses")
    for kl in top_classes:
        print(kl.get_name(), set(kl.all_sub_classes()))

    # create an empty cluster list
    clusters = []
    for kl in top_classes:
        subs = set(kl.all_sub_classes())
        subs.add(kl)
        matches = []
        for cl in clusters:
            if subs & cl:
                matches.append(cl)
        
        if not matches:
            clusters.append(subs)
        else:
            newcl = subs
            for cl in matches:
                clusters.remove(cl)
                newcl |= cl

            clusters.append(newcl)
    print([ [kl.get_name() for kl in cl] for cl in clusters])


              


if __name__ == '__main__':
    cli()