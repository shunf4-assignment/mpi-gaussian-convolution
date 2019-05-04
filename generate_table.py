

conv_kern = [
    [0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881],
	[0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402],
	[0.0350727, 0.06831229, 0.08531173, 0.06831229, 0.0350727],
	[0.02808402, 0.0547002, 0.06831229, 0.0547002, 0.02808402],
	[0.01441881, 0.02808402, 0.0350727, 0.02808402, 0.01441881]
]

conv_pos_index = [
    [0, 1, 2, 1, 0],
    [1, 3, 4, 3, 1],
    [2, 4, 5, 4, 2],
    [1, 3, 4, 3, 1],
    [0, 1, 2, 1, 0],
]

conv_index_weight = {}

for i,y in enumerate(conv_pos_index):
    for j,x in enumerate(y):
        if conv_index_weight.get(x) is None:
            conv_index_weight[x] = conv_kern[i][j]
        elif conv_index_weight.get(x) != conv_kern[i][j]:
            raise ValueError
            
color = list(range(256))

print("double weight_table[7][256] = {")

for k in conv_index_weight:
    print("\t{", end='')
    for c in color:
        print("%.12lg" % (c * conv_index_weight[k]), end=',')
    print("},")

print("\t{", end='')
for c in color:
    print("%.12lg" % 0, end=',')
print("},")

print("}")

