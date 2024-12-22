all:
	make -C Belle2_analysis clean all
	make -C analysis_code clean all

clean:
	make -C Belle2_analysis clean
	make -C analysis_code clean

