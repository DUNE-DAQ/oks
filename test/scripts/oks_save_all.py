#!/usr/bin/env python

from rich import print
import click
import oks

@click.command()
@click.argument("database", type=click.Path(exists=False, dir_okay=False))
def cli(database):

    print(database)

    k = oks.OksKernel(verbose_mode = True)
    k.load_data(database)
    print(f"{database} loaded")

    # k.save_all_data(False)
    k.close_all_data()

if __name__ == '__main__':
    cli()