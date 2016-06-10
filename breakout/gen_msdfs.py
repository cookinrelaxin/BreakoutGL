import sys
import subprocess

font_path = str(sys.argv[1])

# args = '{0} -font {1} {2} -o {3} -size {4} {5} -pxrange {6} -autoframe -testrender {7} {8} {9}'.format(
#         './msdfgen/msdfgen',
#         font_path,
#         65,
#         'test.png',
#         32,
#         32,
#         4,
#         'testrender.png',
#         32,
#         32
#         );

for i in range(33, 127):
    # ascii = str(unichr(i))

    args = '{0} msdf -font {1} {2} -o {3} -size {4} {5} -pxrange {6} -autoframe'.format(
            './msdfgen/msdfgen',
            font_path,
            i,
            './assets/textures/msdfs/' + str(i) + '_msdn.png',
            32,
            32,
            4
            );
    
    subprocess.Popen(args=[args], shell=True);
