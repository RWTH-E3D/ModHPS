<img src="add/LogoModHPS.png" width="250">

# ModHPS

**Mod**el for **H**eat **P**ump **S**ystems (**ModHPS**) is a simplified black-box brine-to-water heat pump system model with the application range from single buildings up to district level. The model contains simplified algorithms for thermal storage balances and control strategies. ModHPS has been developed in the programming language C++ and can be coupled bidirectional to subsurface models. ModHPS is intended to be used especially in early planning and decision making phases where the detailed modeling of dynamics within the heat pump components is not crucial.

## Quick start

Optional: Change input-data in XML (folder 'HPM/import').

Open folder 'HPM/src' in terminal and type:
```
g++ -o ../a.out heatpump.cpp hplink.cpp hps.cpp parser.cpp hpcore.cpp building.cpp UseGNU.cpp PlottingSettings.cpp
```

For co-simulation with an included python script type instead: 
```
g++ -I/usr/include/python2.7 -includePython.h -o ../a.out heatpump.cpp hplink.cpp hps.cpp parser.cpp hpcore.cpp building.cpp UseGNU.cpp PlottingSettings.cpp -lpython2.7 -L/usr/lib/python2.7
```

Open folder 'HPM/run' in terminal and type:
```
../a.out
```

Results in folder 'HPM/run/hpm'. 


## Structure of the repository

Under **src** all scripts and necessary files for the functionality of the model are stored. **import** contains the XML-input file. In **run/hpm** all results are stored. **doc** contains a html documentation (created with doxygen).

## Documentation

Open folder 'HPM/src' in terminal and type:
```
doxygen configFileDoxygen
```
Open folder 'doc/html' and open a html file.


## Citation

The following publication has to be cited for the use of the code.
```bibtex
@article{WeckPonten2022,
  title={Simplified heat pump model integrated in a tool chain for digitally and simulation-based planning shallow geothermal systems},
  author={Weck-Ponten, Sebastian and Frisch, J{\'e}r{\^o}me and {van Treeck}, Christoph},
  journal={Geothermics},
  volume={xx},
  number={xx},
  pages={xx},
  year={2022},
  doi = {xxx},
  publisher={Elsevier}
}
@software{ModHPS,
	title        = {ModHPS},
	author       = {Weck-Ponten, Sebastian},
	url          = {https://github.com/RWTH-E3D/ModHPS},
	version      = {1.0},
	date         = {2023-01-01}
}
```

## Licence

The program code is licensed via the [MIT license](LICENSE).

## Author

[Sebastian Weck-Ponten](https://www.e3d.rwth-aachen.de/cms/E3D/Der-Lehrstuhl/Team/Wissenschaftliche-Beschaeftigte/~mfgk/Sebastian-Weck/)

weck@e3d.rwth-aachen.de  
[E3D - Institute of Energy Efficiency and Sustainable Building  ](https://www.e3d.rwth-aachen.de/go/id/iyld/?)     
RWTH Aachen University

![RWTH Aachen University, E3D](add/E3D_Logo.png)
