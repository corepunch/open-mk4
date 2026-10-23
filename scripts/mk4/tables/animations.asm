; Animation metadata. Each record is indexed by clip(), not by an executable address.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


; Animation descriptors, indexed by clip(): resource field, frame count, packed format.
; Format high 16 bits = decoded words/frame; payload lives in FILESYS.DAT (common/mk4_assets.c).
animation_table:
  .word 0, 13, 0x120000  ; clip 0

animation_clip_1:
  .word 0, 20, 0x120000  ; clip 1

animation_clip_2:
  .word 0, 9, 0x120000  ; clip 2

animation_clip_3:
  .word 0, 20, 0x120000  ; clip 3

animation_clip_4:
  .word 0, 30, 0x120000  ; clip 4

animation_clip_5:
  .word 0, 25, 0x120000  ; clip 5

animation_clip_6:
  .word 0, 13, 0x120000  ; clip 6

animation_clip_7:
  .word 0, 22, 0x120000  ; clip 7

animation_clip_8:
  .word 0, 29, 0x120000  ; clip 8

animation_clip_9:
  .word 0, 12, 0x120000  ; clip 9

animation_clip_10:
  .word 0, 3, 0x120000  ; clip 10

animation_clip_11:
  .word 0, 8, 0x120000  ; clip 11

animation_clip_12:
  .word 0, 58, 0x120000  ; clip 12

animation_clip_13:
  .word 0, 56, 0x120000  ; clip 13

animation_clip_14:
  .word 0, 80, 0x120000  ; clip 14

animation_clip_15:
  .word 0, 20, 0x120000  ; clip 15

animation_clip_16:
  .word 0, 36, 0x120000  ; clip 16

animation_clip_17:
  .word 0, 14, 0x120000  ; clip 17

animation_clip_18:
  .word 0, 45, 0x120000  ; clip 18

animation_clip_19:
  .word 0, 47, 0x120000  ; clip 19

animation_clip_20:
  .word 0, 6, 0x120000  ; clip 20

animation_clip_21:
  .word 0, 7, 0x120000  ; clip 21

animation_clip_22:
  .word 0, 5, 0x120000  ; clip 22

animation_clip_23:
  .word 0, 36, 0x120000  ; clip 23

animation_clip_24:
  .word 0, 10, 0x120000  ; clip 24

animation_clip_25:
  .word 0, 42, 0x120000  ; clip 25

animation_clip_26:
  .word 0, 14, 0x120000  ; clip 26

animation_clip_27:
  .word 0, 22, 0x120000  ; clip 27

animation_clip_28:
  .word 0, 30, 0x120000  ; clip 28

animation_clip_29:
  .word 0, 40, 0x120000  ; clip 29

animation_clip_30:
  .word 0, 62, 0x120000  ; clip 30

animation_clip_31:
  .word 0, 158, 0x120000  ; clip 31

animation_clip_32:
  .word 0, 129, 0x120000  ; clip 32

animation_clip_33:
  .word 0, 77, 0x120000  ; clip 33

animation_clip_34:
  .word 0, 104, 0x120000  ; clip 34

animation_clip_35:
  .word 0, 44, 0x120000  ; clip 35

animation_clip_36:
  .word 0, 104, 0x120000  ; clip 36

animation_clip_37:
  .word 0, 66, 0x120000  ; clip 37

animation_clip_38:
  .word 0, 100, 0x120000  ; clip 38

animation_clip_39:
  .word 0, 29, 0x120000  ; clip 39

animation_clip_40:
  .word 0, 60, 0x120000  ; clip 40

animation_clip_41:
  .word 0, 75, 0x120000  ; clip 41

animation_clip_42:
  .word 0, 128, 0x120000  ; clip 42

animation_clip_43:
  .word 0, 112, 0x120001  ; clip 43

animation_clip_44:
  .word 0, 100, 0x120000  ; clip 44

animation_clip_45:
  .word 0, 150, 0x120000  ; clip 45

animation_clip_46:
  .word 0, 90, 0x120000  ; clip 46

animation_clip_47:
  .word 0, 94, 0x120000  ; clip 47

animation_clip_48:
  .word 0, 102, 0x120000  ; clip 48

animation_clip_49:
  .word 0, 54, 0x120000  ; clip 49

animation_clip_50:
  .word 0, 155, 0x120000  ; clip 50

animation_clip_51:
  .word 0, 117, 0x120000  ; clip 51

animation_clip_52:
  .word 0, 11, 0x120000  ; clip 52

animation_clip_53:
  .word 0, 13, 0x120000  ; clip 53

animation_clip_54:
  .word 0, 13, 0x120000  ; clip 54

animation_clip_55:
  .word 0, 6, 0x120000  ; clip 55

animation_clip_56:
  .word 0, 6, 0x120000  ; clip 56

animation_clip_57:
  .word 0, 6, 0x120000  ; clip 57

animation_clip_58:
  .word 0, 10, 0x120000  ; clip 58

animation_clip_59:
  .word 0, 6, 0x120000  ; clip 59

animation_clip_60:
  .word 0, 6, 0x120000  ; clip 60

animation_clip_61:
  .word 0, 30, 0x120000  ; clip 61

animation_clip_62:
  .word 0, 44, 0x120000  ; clip 62

animation_clip_63:
  .word 0, 46, 0x120000  ; clip 63

animation_clip_64:
  .word 0, 45, 0x120000  ; clip 64

animation_clip_65:
  .word 0, 11, 0x120000  ; clip 65

animation_clip_66:
  .word 0, 18, 0x120000  ; clip 66

animation_clip_67:
  .word 0, 18, 0x120000  ; clip 67

animation_clip_68:
  .word 0, 23, 0x120000  ; clip 68

animation_clip_69:
  .word 0, 61, 0x120000  ; clip 69

animation_clip_70:
  .word 0, 28, 0x120000  ; clip 70

animation_clip_71:
  .word 0, 28, 0x120000  ; clip 71

animation_clip_72:
  .word 0, 9, 0x120000  ; clip 72

animation_clip_73:
  .word 0, 15, 0x120000  ; clip 73

animation_clip_74:
  .word 0, 45, 0x120000  ; clip 74

animation_clip_75:
  .word 0, 65, 0x120000  ; clip 75

animation_clip_76:
  .word 0, 49, 0x120000  ; clip 76

animation_clip_77:
  .word 0, 40, 0x120000  ; clip 77

animation_clip_78:
  .word 0, 45, 0x120000  ; clip 78

animation_clip_79:
  .word 0, 61, 0x120000  ; clip 79

animation_clip_80:
  .word 0, 75, 0x120000  ; clip 80

animation_clip_81:
  .word 0, 79, 0x120000  ; clip 81

animation_clip_82:
  .word 0, 45, 0x120000  ; clip 82

animation_clip_83:
  .word 0, 117, 0x120000  ; clip 83

animation_clip_84:
  .word 0, 72, 0x120000  ; clip 84

animation_clip_85:
  .word 0, 55, 0x120000  ; clip 85

animation_clip_86:
  .word 0, 10, 0x120000  ; clip 86

animation_clip_87:
  .word 0, 20, 0x120000  ; clip 87

animation_clip_88:
  .word 0, 10, 0x120000  ; clip 88

animation_clip_89:
  .word 0, 15, 0x120000  ; clip 89

animation_clip_90:
  .word 0, 13, 0x120000  ; clip 90

animation_clip_91:
  .word 0, 9, 0x120000  ; clip 91

animation_clip_92:
  .word 0, 6, 0x120000  ; clip 92

animation_clip_93:
  .word 0, 3, 0x120000  ; clip 93

animation_clip_94:
  .word 0, 31, 0x120000  ; clip 94

animation_clip_95:
  .word 0, 11, 0x120000  ; clip 95

animation_clip_96:
  .word 0, 11, 0x120000  ; clip 96

animation_clip_97:
  .word 0, 9, 0x120000  ; clip 97

animation_clip_98:
  .word 0, 40, 0x120000  ; clip 98

animation_clip_99:
  .word 0, 30, 0x120000  ; clip 99

animation_clip_100:
  .word 0, 45, 0x120000  ; clip 100

animation_clip_101:
  .word 0, 9, 0x120000  ; clip 101

animation_clip_102:
  .word 0, 65, 0x120000  ; clip 102

animation_clip_103:
  .word 0, 50, 0x120000  ; clip 103

animation_clip_104:
  .word 0, 30, 0x120000  ; clip 104

animation_clip_105:
  .word 0, 30, 0x120000  ; clip 105

animation_clip_106:
  .word 0, 22, 0x120000  ; clip 106

animation_clip_107:
  .word 0, 70, 0x120000  ; clip 107

animation_clip_108:
  .word 0, 57, 0x120000  ; clip 108

animation_clip_109:
  .word 0, 29, 0x120000  ; clip 109

animation_clip_110:
  .word 0, 27, 0x120000  ; clip 110

animation_clip_111:
  .word 0, 136, 0x120000  ; clip 111

animation_clip_112:
  .word 0, 23, 0x120000  ; clip 112

animation_clip_113:
  .word 0, 89, 0x120000  ; clip 113

animation_clip_114:
  .word 0, 100, 0x120000  ; clip 114

animation_clip_115:
  .word 0, 19, 0x120000  ; clip 115

animation_clip_116:
  .word 0, 50, 0x120000  ; clip 116

animation_clip_117:
  .word 0, 19, 0x120000  ; clip 117

animation_clip_118:
  .word 0, 50, 0x120000  ; clip 118

animation_clip_119:
  .word 0, 166, 0x120000  ; clip 119

animation_clip_120:
  .word 0, 120, 0x120000  ; clip 120

animation_clip_121:
  .word 0, 186, 0x120000  ; clip 121

animation_clip_122:
  .word 0, 140, 0x120000  ; clip 122

animation_clip_123:
  .word 0, 101, 0x120000  ; clip 123

animation_clip_124:
  .word 0, 119, 0x120000  ; clip 124

animation_clip_125:
  .word 0, 119, 0x120000  ; clip 125

animation_clip_126:
  .word 0, 15, 0x120000  ; clip 126

animation_clip_127:
  .word 0, 40, 0x120000  ; clip 127

animation_clip_128:
  .word 0, 110, 0x120000  ; clip 128

animation_clip_129:
  .word 0, 70, 0x120000  ; clip 129

animation_clip_130:
  .word 0, 40, 0x120000  ; clip 130

animation_clip_131:
  .word 0, 80, 0x120000  ; clip 131

animation_clip_132:
  .word 0, 60, 0x120000  ; clip 132

animation_clip_133:
  .word 0, 36, 0x120000  ; clip 133

animation_clip_134:
  .word 0, 18, 0x120000  ; clip 134

animation_clip_135:
  .word 0, 10, 0x120000  ; clip 135

animation_clip_136:
  .word 0, 24, 0x120000  ; clip 136

animation_clip_137:
  .word 0, 48, 0x120000  ; clip 137

animation_clip_138:
  .word 0, 22, 0x120000  ; clip 138

animation_clip_139:
  .word 0, 14, 0x120000  ; clip 139

animation_clip_140:
  .word 0, 63, 0x120000  ; clip 140

animation_clip_141:
  .word 0, 17, 0x120000  ; clip 141

animation_clip_142:
  .word 0, 17, 0x120000  ; clip 142

animation_clip_143:
  .word 0, 29, 0x120000  ; clip 143

animation_clip_144:
  .word 0, 44, 0x120001  ; clip 144

animation_clip_145:
  .word 0, 117, 0x120001  ; clip 145

animation_clip_146:
  .word 0, 158, 0x120001  ; clip 146

animation_clip_147:
  .word 0, 124, 0x120001  ; clip 147

animation_clip_148:
  .word 0, 90, 0x120001  ; clip 148

animation_clip_149:
  .word 0, 40, 0x120001  ; clip 149

animation_clip_150:
  .word 0, 28, 0x120001  ; clip 150

animation_clip_151:
  .word 0, 46, 0x120001  ; clip 151

animation_clip_152:
  .word 0, 42, 0x120001  ; clip 152

animation_clip_153:
  .word 0, 26, 0x120001  ; clip 153

animation_clip_154:
  .word 0, 58, 0x120001  ; clip 154

animation_clip_155:
  .word 0, 86, 0x120001  ; clip 155

animation_clip_156:
  .word 0, 22, 0x120001  ; clip 156

animation_clip_157:
  .word 0, 131, 0x120001  ; clip 157

animation_clip_158:
  .word 0, 47, 0x120001  ; clip 158

animation_clip_159:
  .word 0, 73, 0x120001  ; clip 159

animation_clip_160:
  .word 0, 100, 0x120001  ; clip 160

animation_clip_161:
  .word 0, 24, 0x120001  ; clip 161

animation_clip_162:
  .word 0, 138, 0x120001  ; clip 162

animation_clip_163:
  .word 0, 112, 0x120001  ; clip 163

animation_clip_164:
  .word 0, 90, 0x120001  ; clip 164

animation_clip_165:
  .word 0, 47, 0x120001  ; clip 165

animation_clip_166:
  .word 0, 6, 0x120000  ; clip 166

animation_clip_167:
  .word 0, 60, 0x120000  ; clip 167

animation_clip_168:
  .word 0, 29, 0x120000  ; clip 168

animation_clip_169:
  .word 0, 43, 0x120000  ; clip 169

animation_clip_170:
  .word 0, 8, 0x120000  ; clip 170

animation_clip_171:
  .word 0, 35, 0x120000  ; clip 171

animation_clip_172:
  .word 0, 22, 0x120000  ; clip 172

animation_clip_173:
  .word 0, 23, 0x120000  ; clip 173

animation_clip_174:
  .word 0, 27, 0x120000  ; clip 174

animation_clip_175:
  .word 0, 22, 0x120000  ; clip 175

animation_clip_176:
  .word 0, 25, 0x120000  ; clip 176

animation_clip_177:
  .word 0, 60, 0x120000  ; clip 177

animation_clip_178:
  .word 0, 28, 0x120000  ; clip 178

animation_clip_179:
  .word 0, 54, 0x120000  ; clip 179

animation_clip_180:
  .word 0, 20, 0x120000  ; clip 180

animation_clip_181:
  .word 0, 83, 0x120000  ; clip 181

animation_clip_182:
  .word 0, 65, 0x120000  ; clip 182

animation_clip_183:
  .word 0, 34, 0x120000  ; clip 183

animation_clip_184:
  .word 0, 16, 0x120000  ; clip 184

animation_clip_185:
  .word 0, 16, 0x120000  ; clip 185

animation_clip_186:
  .word 0, 20, 0x120000  ; clip 186

animation_clip_187:
  .word 0, 16, 0x120000  ; clip 187

animation_clip_188:
  .word 0, 16, 0x120000  ; clip 188

animation_clip_189:
  .word 0, 30, 0x120000  ; clip 189

animation_clip_190:
  .word 0, 16, 0x120000  ; clip 190

animation_clip_191:
  .word 0, 16, 0x120000  ; clip 191

animation_clip_192:
  .word 0, 35, 0x120000  ; clip 192

animation_clip_193:
  .word 0, 16, 0x120000  ; clip 193

animation_clip_194:
  .word 0, 24, 0x120000  ; clip 194

animation_clip_195:
  .word 0, 29, 0x120000  ; clip 195

animation_clip_196:
  .word 0, 16, 0x120000  ; clip 196

animation_clip_197:
  .word 0, 29, 0x120000  ; clip 197

animation_clip_198:
  .word 0, 41, 0x120000  ; clip 198

animation_clip_199:
  .word 0, 17, 0x120000  ; clip 199

animation_clip_200:
  .word 0, 41, 0x120000  ; clip 200

animation_clip_201:
  .word 0, 58, 0x120000  ; clip 201

animation_clip_202:
  .word 0, 45, 0x120000  ; clip 202

animation_clip_203:
  .word 0, 112, 0x120000  ; clip 203

animation_clip_204:
  .word 0, 386, 0x120000  ; clip 204

animation_clip_205:
  .word 0, 8, 0x120000  ; clip 205

animation_clip_206:
  .word 0, 100, 0x120000  ; clip 206

animation_clip_207:
  .word 0, 5, 0x120000  ; clip 207

animation_clip_208:
  .word 0, 61, 0x120000  ; clip 208

animation_clip_209:
  .word 0, 15, 0x120000  ; clip 209

animation_clip_210:
  .word 0, 37, 0x120000  ; clip 210

animation_clip_211:
  .word 0, 29, 0x120000  ; clip 211

animation_clip_212:
  .word 0, 10, 0x120000  ; clip 212

animation_clip_213:
  .word 0, 30, 0x120000  ; clip 213

animation_clip_214:
  .word 0, 10, 0x120000  ; clip 214

animation_clip_215:
  .word 0, 30, 0x120000  ; clip 215

animation_clip_216:
  .word 0, 22, 0x120000  ; clip 216

animation_clip_217:
  .word 0, 5, 0x120000  ; clip 217

animation_clip_218:
  .word 0, 5, 0x120000  ; clip 218

animation_clip_219:
  .word 0, 33, 0x120000  ; clip 219

animation_clip_220:
  .word 0, 5, 0x120000  ; clip 220

animation_clip_221:
  .word 0, 32, 0x120000  ; clip 221

animation_clip_222:
  .word 0, 5, 0x120000  ; clip 222

animation_clip_223:
  .word 0, 4, 0x120000  ; clip 223

animation_clip_224:
  .word 0, 12, 0x120000  ; clip 224

animation_clip_225:
  .word 0, 10, 0x120000  ; clip 225

animation_clip_226:
  .word 0, 60, 0x120000  ; clip 226

animation_clip_227:
  .word 0, 60, 0x120000  ; clip 227

animation_clip_228:
  .word 0, 40, 0x120000  ; clip 228

animation_clip_229:
  .word 0, 45, 0x120000  ; clip 229

animation_clip_230:
  .word 0, 45, 0x120000  ; clip 230

animation_clip_231:
  .word 0, 45, 0x120000  ; clip 231

animation_clip_232:
  .word 0, 30, 0x120000  ; clip 232

animation_clip_233:
  .word 0, 45, 0x120000  ; clip 233

animation_clip_234:
  .word 0, 45, 0x120000  ; clip 234

animation_clip_235:
  .word 0, 43, 0x120000  ; clip 235

animation_clip_236:
  .word 0, 43, 0x120000  ; clip 236

animation_clip_237:
  .word 0, 45, 0x120000  ; clip 237

animation_clip_238:
  .word 0, 37, 0x120000  ; clip 238

animation_clip_239:
  .word 0, 41, 0x120000  ; clip 239

animation_clip_240:
  .word 0, 45, 0x120000  ; clip 240

animation_clip_241:
  .word 0, 30, 0x120000  ; clip 241

animation_clip_242:
  .word 0, 31, 0x120000  ; clip 242

animation_clip_243:
  .word 0, 28, 0x120000  ; clip 243

animation_clip_244:
  .word 0, 49, 0x120000  ; clip 244

animation_clip_245:
  .word 0, 10, 0x120000  ; clip 245

animation_clip_246:
  .word 0, 31, 0x120000  ; clip 246

animation_clip_247:
  .word 0, 5, 0x120000  ; clip 247

animation_clip_248:
  .word 0, 13, 0x120000  ; clip 248

animation_clip_249:
  .word 0, 12, 0x120000  ; clip 249

animation_clip_250:
  .word 0, 32, 0x120000  ; clip 250

animation_clip_251:
  .word 0, 10, 0x120000  ; clip 251

animation_clip_252:
  .word 0, 10, 0x120000  ; clip 252

animation_clip_253:
  .word 0, 5, 0x120000  ; clip 253

animation_clip_254:
  .word 0, 5, 0x120000  ; clip 254

animation_clip_255:
  .word 0, 5, 0x120000  ; clip 255

animation_clip_256:
  .word 0, 39, 0x120000  ; clip 256

animation_clip_257:
  .word 0, 5, 0x120000  ; clip 257

animation_clip_258:
  .word 0, 39, 0x120000  ; clip 258

animation_clip_259:
  .word 0, 5, 0x120000  ; clip 259

animation_clip_260:
  .word 0, 10, 0x120000  ; clip 260

animation_clip_261:
  .word 0, 28, 0x120000  ; clip 261

animation_clip_262:
  .word 0, 10, 0x120000  ; clip 262

animation_clip_263:
  .word 0, 10, 0x120000  ; clip 263

animation_clip_264:
  .word 0, 10, 0x120000  ; clip 264

animation_clip_265:
  .word 0, 30, 0x120000  ; clip 265

animation_clip_266:
  .word 0, 30, 0x120000  ; clip 266

animation_clip_267:
  .word 0, 30, 0x120000  ; clip 267

animation_clip_268:
  .word 0, 30, 0x120000  ; clip 268

animation_clip_269:
  .word 0, 30, 0x120000  ; clip 269

animation_clip_270:
  .word 0, 30, 0x120000  ; clip 270

animation_clip_271:
  .word 0, 30, 0x120000  ; clip 271

animation_clip_272:
  .word 0, 30, 0x120000  ; clip 272

animation_clip_273:
  .word 0, 30, 0x120000  ; clip 273

animation_clip_274:
  .word 0, 30, 0x120000  ; clip 274

animation_clip_275:
  .word 0, 30, 0x120000  ; clip 275

animation_clip_276:
  .word 0, 30, 0x120000  ; clip 276

animation_clip_277:
  .word 0, 30, 0x120000  ; clip 277

animation_clip_278:
  .word 0, 30, 0x120000  ; clip 278

animation_clip_279:
  .word 0, 30, 0x120000  ; clip 279

animation_clip_280:
  .word 0, 30, 0x120000  ; clip 280

animation_clip_281:
  .word 0, 30, 0x120000  ; clip 281

animation_clip_282:
  .word 0, 30, 0x120000  ; clip 282

animation_clip_283:
  .word 0, 45, 0x120000  ; clip 283

animation_clip_284:
  .word 0, 40, 0x120000  ; clip 284

animation_clip_285:
  .word 0, 35, 0x120000  ; clip 285

animation_clip_286:
  .word 0, 35, 0x120000  ; clip 286

animation_clip_287:
  .word 0, 45, 0x120000  ; clip 287

animation_clip_288:
  .word 0, 50, 0x120000  ; clip 288

animation_clip_289:
  .word 0, 49, 0x120000  ; clip 289

animation_clip_290:
  .word 0, 35, 0x120000  ; clip 290

animation_clip_291:
  .word 0, 73, 0x120000  ; clip 291

animation_clip_292:
  .word 0, 35, 0x120000  ; clip 292

animation_clip_293:
  .word 0, 20, 0x120000  ; clip 293

animation_clip_294:
  .word 0, 23, 0x120000  ; clip 294

animation_clip_295:
  .word 0, 19, 0x120000  ; clip 295

animation_clip_296:
  .word 0, 21, 0x120000  ; clip 296

animation_clip_297:
  .word 0, 19, 0x120000  ; clip 297

animation_clip_298:
  .word 0, 20, 0x120000  ; clip 298

animation_clip_299:
  .word 0, 35, 0x120000  ; clip 299

animation_clip_300:
  .word 0, 18, 0x120000  ; clip 300

animation_clip_301:
  .word 0, 9, 0x120000  ; clip 301

animation_clip_302:
  .word 0, 8, 0x120000  ; clip 302

animation_clip_303:
  .word 0, 20, 0x120000  ; clip 303

animation_clip_304:
  .word 0, 28, 0x120000  ; clip 304

animation_clip_305:
  .word 0, 3, 0x120000  ; clip 305

animation_clip_306:
  .word 0, 14, 0x120000  ; clip 306

animation_clip_307:
  .word 0, 24, 0x120000  ; clip 307

animation_clip_308:
  .word 0, 25, 0x120000  ; clip 308

animation_clip_309:
  .word 0, 15, 0x120000  ; clip 309

animation_clip_310:
  .word 0, 28, 0x120000  ; clip 310

animation_clip_311:
  .word 0, 19, 0x120000  ; clip 311

animation_clip_312:
  .word 0, 10, 0x120000  ; clip 312

animation_clip_313:
  .word 0, 39, 0x120000  ; clip 313

animation_clip_314:
  .word 0, 13, 0x120000  ; clip 314

animation_clip_315:
  .word 0, 10, 0x120000  ; clip 315

animation_clip_316:
  .word 0, 20, 0x120000  ; clip 316

animation_clip_317:
  .word 0, 8, 0x120000  ; clip 317

animation_clip_318:
  .word 0, 8, 0x120000  ; clip 318

animation_clip_319:
  .word 0, 83, 0x120000  ; clip 319

animation_clip_320:
  .word 0, 68, 0x120000  ; clip 320

animation_clip_321:
  .word 0, 33, 0x120000  ; clip 321

animation_clip_322:
  .word 0, 5, 0x120000  ; clip 322

animation_clip_323:
  .word 0, 38, 0x120000  ; clip 323

animation_clip_324:
  .word 0, 14, 0x120000  ; clip 324

animation_clip_325:
  .word 0, 14, 0x120000  ; clip 325

animation_clip_326:
  .word 0, 20, 0x120000  ; clip 326

animation_clip_327:
  .word 0, 8, 0x120000  ; clip 327

animation_clip_328:
  .word 0, 15, 0x120000  ; clip 328

animation_clip_329:
  .word 0, 42, 0x120000  ; clip 329

animation_clip_330:
  .word 0, 20, 0x120000  ; clip 330

animation_clip_331:
  .word 0, 36, 0x120000  ; clip 331

animation_clip_332:
  .word 0, 45, 0x120000  ; clip 332

animation_clip_333:
  .word 0, 45, 0x120000  ; clip 333

animation_clip_334:
  .word 0, 17, 0x120000  ; clip 334

animation_clip_335:
  .word 0, 40, 0x120000  ; clip 335

animation_clip_336:
  .word 0, 40, 0x120000  ; clip 336

animation_clip_337:
  .word 0, 63, 0x120000  ; clip 337

animation_clip_338:
  .word 0, 59, 0x120000  ; clip 338

animation_clip_339:
  .word 0, 59, 0x120000  ; clip 339

animation_clip_340:
  .word 0, 33, 0x120000  ; clip 340

animation_clip_341:
  .word 0, 179, 0x120000  ; clip 341

animation_clip_342:
  .word 0, 52, 0x120000  ; clip 342

animation_clip_343:
  .word 0, 73, 0x120000  ; clip 343

animation_clip_344:
  .word 0, 54, 0x120000  ; clip 344

animation_clip_345:
  .word 0, 100, 0x120000  ; clip 345

animation_clip_346:
  .word 0, 71, 0x120000  ; clip 346

animation_clip_347:
  .word 0, 84, 0x120000  ; clip 347

animation_clip_348:
  .word 0, 130, 0x120000  ; clip 348

animation_clip_349:
  .word 0, 52, 0x120000  ; clip 349

animation_clip_350:
  .word 0, 30, 0x120000  ; clip 350

animation_clip_351:
  .word 0, 59, 0x120000  ; clip 351

animation_clip_352:
  .word 0, 150, 0x120000  ; clip 352

animation_clip_353:
  .word 0, 49, 0x120000  ; clip 353

animation_clip_354:
  .word 0, 60, 0x120000  ; clip 354

animation_clip_355:
  .word 0, 57, 0x120000  ; clip 355

animation_clip_356:
  .word 0, 151, 0x120000  ; clip 356

animation_clip_357:
  .word 0, 78, 0x120000  ; clip 357

animation_clip_358:
  .word 0, 43, 0x120000  ; clip 358

animation_clip_359:
  .word 0, 76, 0x120000  ; clip 359

animation_clip_360:
  .word 0, 121, 0x120000  ; clip 360

animation_clip_361:
  .word 0, 76, 0x120000  ; clip 361

animation_clip_362:
  .word 0, 164, 0x120000  ; clip 362

animation_clip_363:
  .word 0, 31, 0x120000  ; clip 363

animation_clip_364:
  .word 0, 76, 0x120000  ; clip 364

animation_clip_365:
  .word 0, 17, 0x120000  ; clip 365

animation_clip_366:
  .word 0, 10, 0x120000  ; clip 366

animation_clip_367:
  .word 0, 45, 0x120000  ; clip 367

animation_clip_368:
  .word 0, 16, 0x120000  ; clip 368

animation_clip_369:
  .word 0, 146, 0x120000  ; clip 369

animation_clip_370:
  .word 0, 103, 0x120000  ; clip 370

animation_clip_371:
  .word 0, 30, 0x120000  ; clip 371

animation_clip_372:
  .word 0, 40, 0x120000  ; clip 372

animation_clip_373:
  .word 0, 20, 0x120000  ; clip 373

animation_clip_374:
  .word 0, 138, 0x120000  ; clip 374

animation_clip_375:
  .word 0, 100, 0x120000  ; clip 375

animation_clip_376:
  .word 0, 37, 0x120000  ; clip 376

animation_clip_377:
  .word 0, 37, 0x120000  ; clip 377

animation_clip_378:
  .word 0, 35, 0x120000  ; clip 378

animation_clip_379:
  .word 0, 50, 0x120000  ; clip 379

animation_clip_380:
  .word 0, 47, 0x120000  ; clip 380

animation_clip_381:
  .word 0, 10, 0x120000  ; clip 381

animation_clip_382:
  .word 0, 91, 0x120000  ; clip 382

animation_clip_383:
  .word 0, 50, 0x120000  ; clip 383

animation_clip_384:
  .word 0, 250, 0x120000  ; clip 384

animation_clip_385:
  .word 0, 141, 0x120000  ; clip 385

animation_clip_386:
  .word 0, 97, 0x90000  ; clip 386

animation_clip_387:
  .word 0, 74, 0x120000  ; clip 387

animation_clip_388:
  .word 0, 129, 0x120000  ; clip 388

animation_clip_389:
  .word 0, 1, 0x120000  ; clip 389

animation_clip_390:
  .word 0, 145, 0x120000  ; clip 390

animation_clip_391:
  .word 0, 170, 0x120000  ; clip 391

animation_clip_392:
  .word 0, 25, 0x120000  ; clip 392

animation_clip_393:
  .word 0, 25, 0x120000  ; clip 393

animation_clip_394:
  .word 0, 15, 0x120000  ; clip 394

animation_clip_395:
  .word 0, 141, 0x120000  ; clip 395

animation_clip_396:
  .word 0, 77, 0x120000  ; clip 396

animation_clip_397:
  .word 0, 196, 0x120000  ; clip 397

animation_clip_398:
  .word 0, 27, 0x120000  ; clip 398

animation_clip_399:
  .word 0, 18, 0x120000  ; clip 399

animation_clip_400:
  .word 0, 386, 0x120000  ; clip 400

animation_clip_401:
  .word 0, 18, 0x120000  ; clip 401

animation_clip_402:
  .word 0, 238, 0x120000  ; clip 402

animation_clip_403:
  .word 0, 238, 0x120000  ; clip 403

animation_clip_404:
  .word 0, 205, 0x120000  ; clip 404

animation_clip_405:
  .word 0, 61, 0x120000  ; clip 405

animation_clip_406:
  .word 0, 377, 0x120000  ; clip 406

animation_clip_407:
  .word 0, 377, 0x120000  ; clip 407

animation_clip_408:
  .word 0, 143, 0x120000  ; clip 408

animation_clip_409:
  .word 0, 75, 0x120000  ; clip 409

animation_clip_410:
  .word 0, 310, 0x120000  ; clip 410

animation_clip_411:
  .word 0, 310, 0x120000  ; clip 411

animation_clip_412:
  .word 0, 49, 0x120000  ; clip 412

animation_clip_413:
  .word 0, 1, 0x120000  ; clip 413

animation_clip_414:
  .word 0, 9, 0x120000  ; clip 414

animation_clip_415:
  .word 0, 130, 0x120000  ; clip 415

animation_clip_416:
  .word 0, 108, 0x120000  ; clip 416

animation_clip_417:
  .word 0, 30, 0x120000  ; clip 417

animation_clip_418:
  .word 0, 519, 0xd0000  ; clip 418

animation_clip_419:
  .word 0, 289, 0x120000  ; clip 419

animation_clip_420:
  .word 0, 252, 0x120000  ; clip 420

animation_clip_421:
  .word 0, 30, 0x120000  ; clip 421

animation_clip_422:
  .word 0, 135, 0x60000  ; clip 422

animation_clip_423:
  .word 0, 30, 0x60000  ; clip 423

animation_clip_424:
  .word 0, 252, 0x120000  ; clip 424

animation_clip_425:
  .word 0, 94, 0x120000  ; clip 425

animation_clip_426:
  .word 0, 89, 0x60000  ; clip 426

animation_clip_427:
  .word 0, 70, 0x60000  ; clip 427

animation_clip_428:
  .word 0, 194, 0x120000  ; clip 428

animation_clip_429:
  .word 0, 51, 0x40000  ; clip 429

animation_clip_430:
  .word 0, 125, 0x40000  ; clip 430

animation_clip_431:
  .word 0, 415, 0x120000  ; clip 431

animation_clip_432:
  .word 0, 339, 0x120000  ; clip 432

animation_clip_433:
  .word 0, 258, 0x120000  ; clip 433

animation_clip_434:
  .word 0, 380, 0x140000  ; clip 434

animation_clip_435:
  .word 0, 281, 0x120000  ; clip 435

animation_clip_436:
  .word 0, 74, 0x120000  ; clip 436

animation_clip_437:
  .word 0, 24, 0x120000  ; clip 437

animation_clip_438:
  .word 0, 15, 0x120000  ; clip 438

animation_clip_439:
  .word 0, 45, 0x120000  ; clip 439

animation_clip_440:
  .word 0, 440, 0x120000  ; clip 440

animation_clip_441:
  .word 0, 402, 0x120000  ; clip 441

animation_clip_442:
  .word 0, 336, 0x120000  ; clip 442

animation_clip_443:
  .word 0, 30, 0x120000  ; clip 443

animation_clip_444:
  .word 0, 31, 0x120000  ; clip 444

animation_clip_445:
  .word 0, 31, 0x120000  ; clip 445

animation_clip_446:
  .word 0, 201, 0x60000  ; clip 446

animation_clip_447:
  .word 0, 151, 0x60000  ; clip 447

animation_clip_448:
  .word 0, 35, 0x120000  ; clip 448

animation_clip_449:
  .word 0, 1, 0x120000  ; clip 449

animation_clip_450:
  .word 0, 1, 0x120000  ; clip 450

animation_clip_451:
  .word 0, 1, 0x120000  ; clip 451

animation_clip_452:
  .word 0, 1, 0x120000  ; clip 452

animation_clip_453:
  .word 0, 1, 0x120000  ; clip 453

animation_clip_454:
  .word 0, 1, 0x120000  ; clip 454

animation_clip_455:
  .word 0, 1, 0x120000  ; clip 455

animation_clip_456:
  .word 0, 1, 0x120000  ; clip 456

animation_clip_457:
  .word 0, 1, 0x120000  ; clip 457

animation_clip_458:
  .word 0, 74, 0x120000  ; clip 458

animation_clip_459:
  .word 0, 85, 0x120000  ; clip 459

animation_clip_460:
  .word 0, 43, 0x120000  ; clip 460

animation_clip_461:
  .word 0, 130, 0x120000  ; clip 461

animation_clip_462:
  .word 0, 56, 0x120000  ; clip 462

animation_clip_463:
  .word 0, 189, 0x120000  ; clip 463

animation_clip_464:
  .word 0, 133, 0x120000  ; clip 464

animation_clip_465:
  .word 0, 180, 0x120000  ; clip 465

animation_clip_466:
  .word 0, 133, 0x120000  ; clip 466

animation_clip_467:
  .word 0, 29, 0x120000  ; clip 467

animation_clip_468:
  .word 0, 171, 0x120000  ; clip 468

animation_clip_469:
  .word 0, 221, 0x120000  ; clip 469

animation_clip_470:
  .word 0, 39, 0x120000  ; clip 470

animation_clip_471:
  .word 0, 65, 0x120000  ; clip 471

animation_clip_472:
  .word 0, 44, 0x120000  ; clip 472

animation_clip_473:
  .word 0, 268, 0x120000  ; clip 473

animation_clip_474:
  .word 0, 50, 0x120000  ; clip 474

animation_clip_475:
  .word 0, 40, 0x120000  ; clip 475

animation_clip_476:
  .word 0, 132, 0x120000  ; clip 476

animation_clip_477:
  .word 0, 161, 0x120000  ; clip 477

animation_clip_478:
  .word 0, 209, 0x120000  ; clip 478

animation_clip_479:
  .word 0, 31, 0x120000  ; clip 479

animation_clip_480:
  .word 0, 61, 0x120000  ; clip 480

animation_clip_481:
  .word 0, 64, 0x120000  ; clip 481

animation_clip_482:
  .word 0, 87, 0x120000  ; clip 482

animation_clip_483:
  .word 0, 21, 0x120000  ; clip 483

animation_clip_484:
  .word 0, 16, 0x120000  ; clip 484

animation_clip_485:
  .word 0, 36, 0x120000  ; clip 485

animation_clip_486:
  .word 0, 55, 0x120000  ; clip 486

animation_clip_487:
  .word 0, 20, 0x120000  ; clip 487

animation_clip_488:
  .word 0, 35, 0x120000  ; clip 488

animation_clip_489:
  .word 0, 17, 0x120000  ; clip 489

animation_clip_490:
  .word 0, 40, 0x120000  ; clip 490

animation_clip_491:
  .word 0, 19, 0x120000  ; clip 491

animation_clip_492:
  .word 0, 21, 0x120000  ; clip 492

animation_clip_493:
  .word 0, 40, 0x120000  ; clip 493

animation_clip_494:
  .word 0, 45, 0x120000  ; clip 494

animation_clip_495:
  .word 0, 55, 0x120000  ; clip 495

animation_clip_496:
  .word 0, 160, 0x120000  ; clip 496

animation_clip_497:
  .word 0, 83, 0x120000  ; clip 497

animation_clip_498:
  .word 0, 10, 0x120000  ; clip 498

animation_clip_499:
  .word 0, 90, 0x120000  ; clip 499

animation_clip_500:
  .word 0, 10, 0x120000  ; clip 500

animation_clip_501:
  .word 0, 40, 0x120000  ; clip 501

animation_clip_502:
  .word 0, 100, 0x120000  ; clip 502

animation_clip_503:
  .word 0, 10, 0x120000  ; clip 503

animation_clip_504:
  .word 0, 80, 0x120000  ; clip 504

animation_clip_505:
  .word 0, 40, 0x120000  ; clip 505

animation_clip_506:
  .word 0, 57, 0x120000  ; clip 506

animation_clip_507:
  .word 0, 30, 0x120000  ; clip 507

animation_clip_508:
  .word 0, 67, 0x120000  ; clip 508

animation_clip_509:
  .word 0, 21, 0x120000  ; clip 509

animation_clip_510:
  .word 0, 36, 0x120000  ; clip 510

animation_clip_511:
  .word 0, 1, 0x120000  ; clip 511

animation_clip_512:
  .word 0, 40, 0x120000  ; clip 512

animation_clip_513:
  .word 0, 10, 0x120000  ; clip 513

animation_clip_514:
  .word 0, 1, 0x120000  ; clip 514

animation_clip_515:
  .word 0, 20, 0x120000  ; clip 515

animation_clip_516:
  .word 0, 19, 0x120000  ; clip 516

animation_clip_517:
  .word 0, 20, 0x120000  ; clip 517

animation_clip_518:
  .word 0, 169, 0x120000  ; clip 518

animation_clip_519:
  .word 0, 55, 0x120000  ; clip 519

animation_clip_520:
  .word 0, 107, 0x120000  ; clip 520

animation_clip_521:
  .word 0, 50, 0x120000  ; clip 521

animation_clip_522:
  .word 0, 11, 0x120000  ; clip 522

animation_clip_523:
  .word 0, 34, 0x120000  ; clip 523

animation_clip_524:
  .word 0, 100, 0x120000  ; clip 524

animation_clip_525:
  .word 0, 1, 0x120000  ; clip 525

animation_clip_526:
  .word 0, 41, 0x120000  ; clip 526

animation_clip_527:
  .word 0, 27, 0x120000  ; clip 527

animation_clip_528:
  .word 0, 95, 0x120000  ; clip 528

animation_clip_529:
  .word 0, 76, 0x120000  ; clip 529

animation_clip_530:
  .word 0, 120, 0x120000  ; clip 530

animation_clip_531:
  .word 0, 140, 0x120000  ; clip 531

animation_clip_532:
  .word 0, 29, 0x120000  ; clip 532

animation_clip_533:
  .word 0, 24, 0x120000  ; clip 533

animation_clip_534:
  .word 0, 45, 0x120000  ; clip 534

animation_clip_535:
  .word 0, 24, 0x120001  ; clip 535

animation_clip_536:
  .word 0, 65, 0x120000  ; clip 536

animation_clip_537:
  .word 0, 60, 0x120000  ; clip 537

animation_clip_538:
  .word 0, 17, 0x120000  ; clip 538

animation_clip_539:
  .word 0, 38, 0x120001  ; clip 539

animation_clip_540:
  .word 0, 21, 0x120000  ; clip 540

animation_clip_541:
  .word 0, 200, 0x140000  ; clip 541

animation_clip_542:
  .word 0, 12, 0x120000  ; clip 542

animation_clip_543:
  .word 0, 40, 0x120000  ; clip 543

animation_clip_544:
  .word 0, 16, 0x120000  ; clip 544

animation_clip_545:
  .word 0, 10, 0x120000  ; clip 545

animation_clip_546:
  .word 0, 107, 0x120000  ; clip 546

animation_clip_547:
  .word 0, 77, 0x120000  ; clip 547

animation_clip_548:
  .word 0, 9, 0x120000  ; clip 548

animation_clip_549:
  .word 0, 56, 0x120000  ; clip 549

animation_clip_550:
  .word 0, 66, 0x120000  ; clip 550

animation_clip_551:
  .word 0, 60, 0x120000  ; clip 551

animation_clip_552:
  .word 0, 105, 0x120000  ; clip 552

animation_clip_553:
  .word 0, 51, 0x120000  ; clip 553

animation_clip_554:
  .word 0, 160, 0x120000  ; clip 554

animation_clip_555:
  .word 0, 40, 0x120000  ; clip 555

animation_clip_556:
  .word 0, 34, 0x120000  ; clip 556

animation_clip_557:
  .word 0, 39, 0x120000  ; clip 557

animation_clip_558:
  .word 0, 16, 0x120000  ; clip 558

animation_clip_559:
  .word 0, 79, 0x120000  ; clip 559

animation_clip_560:
  .word 0, 73, 0x120000  ; clip 560

animation_clip_561:
  .word 0, 79, 0x120000  ; clip 561

animation_clip_562:
  .word 0, 41, 0x120000  ; clip 562

animation_clip_563:
  .word 0, 51, 0x120000  ; clip 563

animation_clip_564:
  .word 0, 19, 0x120000  ; clip 564

animation_clip_565:
  .word 0, 61, 0x120000  ; clip 565

animation_clip_566:
  .word 0, 73, 0x120000  ; clip 566

animation_clip_567:
  .word 0, 19, 0x120000  ; clip 567

animation_clip_568:
  .word 0, 61, 0x120000  ; clip 568

animation_clip_569:
  .word 0, 204, 0x120000  ; clip 569

animation_clip_570:
  .word 0, 39, 0x120000  ; clip 570

animation_clip_571:
  .word 0, 15, 0x120000  ; clip 571

animation_clip_572:
  .word 0, 76, 0x120000  ; clip 572

animation_clip_573:
  .word 0, 21, 0x120000  ; clip 573

animation_clip_574:
  .word 0, 39, 0x120000  ; clip 574

animation_clip_575:
  .word 0, 15, 0x120000  ; clip 575

animation_clip_576:
  .word 0, 65, 0x120000  ; clip 576

animation_clip_577:
  .word 0, 115, 0x120000  ; clip 577

animation_clip_578:
  .word 0, 49, 0x180000  ; clip 578

animation_clip_579:
  .word 0, 35, 0x180000  ; clip 579

animation_clip_580:
  .word 0, 78, 0x180000  ; clip 580

animation_clip_581:
  .word 0, 38, 0x180000  ; clip 581

animation_clip_582:
  .word 0, 49, 0x120000  ; clip 582

animation_clip_583:
  .word 0, 38, 0x120000  ; clip 583

animation_clip_584:
  .word 0, 78, 0x120000  ; clip 584

animation_clip_585:
  .word 0, 48, 0x120000  ; clip 585

animation_clip_586:
  .word 0, 131, 0x120000  ; clip 586

animation_clip_587:
  .word 0, 47, 0x180000  ; clip 587

animation_clip_588:
  .word 0, 24, 0x180000  ; clip 588

animation_clip_589:
  .word 0, 48, 0x180000  ; clip 589

animation_clip_590:
  .word 0, 38, 0x180000  ; clip 590

animation_clip_591:
  .word 0, 32, 0x180000  ; clip 591

animation_clip_592:
  .word 0, 25, 0x180000  ; clip 592

animation_clip_593:
  .word 0, 33, 0x180000  ; clip 593

animation_clip_594:
  .word 0, 118, 0x180000  ; clip 594

animation_clip_595:
  .word 0, 5, 0x180000  ; clip 595

animation_clip_596:
  .word 0, 102, 0x180000  ; clip 596

animation_clip_597:
  .word 0, 55, 0x180000  ; clip 597

animation_clip_598:
  .word 0, 31, 0x180000  ; clip 598

animation_clip_599:
  .word 0, 57, 0x180000  ; clip 599

animation_clip_600:
  .word 0, 37, 0x180000  ; clip 600

animation_clip_601:
  .word 0, 52, 0x180000  ; clip 601

animation_clip_602:
  .word 0, 30, 0x180000  ; clip 602

animation_clip_603:
  .word 0, 19, 0x180000  ; clip 603

animation_clip_604:
  .word 0, 48, 0x180000  ; clip 604

animation_clip_605:
  .word 0, 24, 0x180000  ; clip 605

animation_clip_606:
  .word 0, 48, 0x120001  ; clip 606

animation_clip_607:
  .word 0, 24, 0x120001  ; clip 607

animation_clip_608:
  .word 0, 60, 0x180000  ; clip 608

animation_clip_609:
  .word 0, 141, 0x180000  ; clip 609

animation_clip_610:
  .word 0, 141, 0x120000  ; clip 610

animation_clip_611:
  .word 0, 36, 0x120000  ; clip 611
