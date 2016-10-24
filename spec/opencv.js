var cv = require('../index.js');
var assert = require('assert');
var fs = require('fs');

describe('Smoke OpenCV', function () {
  it('should import nicely', function (done) {
    cv = require('../index')
    assert.ok(cv, "imported fine")
    assert.ok(cv.version, "version is there:" + cv.version)
    assert.ok(cv.Matrix, "matrix is there")
    done();
  });

  it('should import multiple times', function(done){
    var cv1 = require('../index')
      , cv2 = require('../index');

    cv1.readImage('spec/fixtures/screen.png', function(err, im){
      assert.ifError(err);
      cv2.readImage('spec/fixtures/screen.png', function(err, im){
        assert.ifError(err);
        done();
      });
    });
  });

  it("should read from file", function(done){
    cv.readImage("spec/fixtures/screen.png", function(err, im){
      assert.ok(im);
      assert.equal(im.width(), 480);
      assert.equal(im.height(), 800);
      assert.equal(im.channels(), 3);
      assert.equal(im.empty(), false);
      done();
    })
  });

  it("read Image from buffer", function(done){
    fs.readFile('spec/fixtures/screen.png', function(err,file){
      cv.readImage(file, function(err, im){
        assert.ok(im);
        assert.equal(im.width(), 480);
        assert.equal(im.height(), 800);
        assert.equal(im.channels(), 3);
        assert.equal(im.empty(), false);
        done();
      });
    });
  });

  it("should have ImageStream working", function(done){
    var s = new cv.ImageStream()
      , im = fs.readFileSync('spec/fixtures/screen.png');

    s.on('data', function(mat){
      assert.deepEqual(mat.size(), [800, 480]);
      done();
    })
    s.write(im);
  });

  it("should have ImageDataStream working", function(done){
    var s = new cv.ImageDataStream()
    s.on('load', function(im){
      assert.ok(im)
      assert.equal(im.empty(), false);
      done();
    });

    fs.createReadStream('spec/fixtures/screen.png').pipe(s);
  });

  it("should write to buffer (sync)", function(done){
    var buf = fs.readFileSync('spec/fixtures/screen.png');
    cv.readImage(buf.slice(0), function(err, mat){
      var buf0 = mat.toBuffer();
      assert.ok(buf0);
      done();
    });
  });

  it("should write to buffer (async)", function(done){
    fs.readFile('spec/fixtures/screen.png', function(err,buf){
      cv.readImage(buf.slice(0), function(err, mat){
        mat.toBuffer(function(err, buff){
          assert.ifError(err)
          assert.ok(buf)
          assert.ok(buf.length > 100)
          done();
        });
      });
    });
  });
});
