#!/usr/bin/python3
# -*- coding: utf-8

"""
Convert Hayai JSON output to CSV (for use in spreadsheets).
"""

import argparse
import csv
import json
import sys

if __name__ == "__main__":
	
	# Argument parsing
	parser = argparse.ArgumentParser()
	parser.add_argument('infile',help="JSON file to read")
	parser.add_argument('outfile',help="CSV file to write")
	args = parser.parse_args()
	
	# Determine CSV columns
	cols = ['fixture','name','parameters','iterations_per_run','disabled','runs','mean','std_dev','median','quartile_1','quartile_3','minimum','maximum']

	with open(args.infile, 'r') as infile, open(args.outfile,'w',newline='') as outfile:
		
		# Write CSV header row
		outwriter = csv.writer(outfile,delimiter=',',quoting=csv.QUOTE_NONNUMERIC)
		outwriter.writerow(cols)
		
		# Ingest JSON data
		json_data = json.load(infile)
		
		# Iterate across benchmarks in the JSON data
		benchmarks = json_data['benchmarks']
		for benchmark in benchmarks:
			# Lookup values for the row from benchmark dictionary
			csv_row = list(map(benchmark.get,cols))
			
			# Assemble a parameter string if parameters existed in this benchmark item
			if 'parameters' in benchmark.keys():
				csv_row[2] = " ".join([parameter['declaration']+"="+parameter['value'] for parameter in benchmark['parameters']])
				
			# Count runs. Find minimum and maximum run durations
			runs = [run['duration'] for run in benchmark['runs']]
			csv_row[5] = len(runs)
			csv_row[-2] = min(runs)
			csv_row[-1] = max(runs)
			
			# Write the CSV row
			outwriter.writerow(csv_row)
	